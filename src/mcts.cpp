#include "mcts.hpp"

MCTS::MCTS(std::shared_ptr<Settings> settings, std::unique_ptr<Node> root, std::shared_ptr<NeuralNetwork> const & nn)
  : m_Settings(settings)
  , m_NN(nn)
{

    if (root == nullptr)
    {
        root = std::make_unique<Node>(std::make_shared<Environment>(m_Settings->getRows(), m_Settings->getCols()));
    }
    setRoot(std::move(root));

    // uses torch::kCPU if useCUDA is false
    if (m_Settings->useCUDA())
    {
        m_Device = torch::kCUDA;
    }
}

MCTS::~MCTS() {
    LDEBUG << "Destroying MCTS";
}

std::unique_ptr<Node> const & MCTS::getRoot() const
{
    return m_Root;
}

void MCTS::setRoot(Node * newRoot)
{
    // release the child from previousNode where child == newroot
    Node * previousNode = newRoot->getParent();
    for (auto const & child: previousNode->getChildren())
    {
        if (child.get() == newRoot)
        {
            newRoot = previousNode->removeChild(child);
            break;
        }
    }

    // now we can reset the current root, deleting all unnecessary children
    m_Root.reset();

    // now set the new root
    m_Root = std::unique_ptr<Node>(newRoot);
    m_Root->setParent(nullptr);
}

void MCTS::setRoot(std::unique_ptr<Node> newRoot)
{
    m_Root = std::move(newRoot);
    m_Root->setParent(nullptr);
}

void MCTS::addDirichletNoise(Node * root)
{
    if (root->getParent() == nullptr)
    {
        auto input  = m_NN->boardToInput(root->getEnvironment());
        auto output = m_NN->predict(input);
        auto policy = output.first.view({7});
        // root node, add dirichlet noise to policy
        auto noise = utils::calculateDirichletNoise(policy);
        float frac = 0.40;
        for (int i = 0; i < policy.size(0); i++)
        {
            std::stringstream ss;
            ss << "Prior " << i << ": " << policy[i].item<float>();
            root->setPrior((policy[i] * (1 - frac) + noise[i] * frac).item<float>());
            ss << "\t => \t" << root->getPrior();
            LDEBUG << ss.str();
        }
    }
    else
    {
        LFATAL << "Root has parent, this shouldn't be the case when adding dirichlet noise";
    }
}

void MCTS::run_simulations(int simulations)
{
    Node* root = getRoot().get();

    // TODO: only add this in selfplay
    addDirichletNoise(root);

    LINFO << "Running " << simulations << " simulations...\n";
    tqdm bar;
    for (int i = 0; i < simulations && g_Running; i++)
    {
        bar.progress(i, simulations);
        // step 1: selection
        Node * selected = select(root);
        // step 2 and 3: expansion and evaluation
        float result = expand(selected);
        // step 4: backpropagation
        backpropagate(selected, result);
    }
    // std::cout << "Tree depth: " << MCTS::getTreeDepth(root);
    std::cout << std::endl;
}

Node * MCTS::select(Node* root)
{
    // keep selecting nodes using the Q+U formula
    // until we reach a node not yet expanded
    Node * current = root;
    while (current->getChildren().size() > 0)
    {
        Node * best_child = nullptr;
        float  best_score = -2;
        for (auto & child: current->getChildren())
        {
            float score = child->getQ() + child->getU();
            if (score > best_score)
            {
                best_child = child.get();
                best_score = score;
            }
        }
        if (best_child == nullptr)
        {
            LFATAL << "Error: best child is null";
        }
        current = best_child;
    }
    return current;
}

float MCTS::expand(Node * node)
{
    // expand the node by adding a child for each possible move
    std::shared_ptr<Environment> const & env = node->getEnvironment();

    torch::Tensor                           input  = m_NN->boardToInput(env);
    std::pair<torch::Tensor, torch::Tensor> output = m_NN->predict(input);

    // policy output
    torch::Tensor policy = output.first.view({7});
    // value output (= step 3: evaluation)
    float value = output.second.item<float>();

    std::vector<int> valid_moves = env->getValidMoves();
    if (valid_moves.size() == 0)
    {
        return env->getWinner() == ePlayer::NONE ? 0 : 1;
    }

    if (env->getWinner() != ePlayer::NONE)
    {
        return 1;
    }

    // add a child node to the leaf node for every possible action (= step 2: expansion)
    for (auto const & move: valid_moves)
    {
        // copy the environment and make the new move
        std::shared_ptr<Environment> new_env = std::make_shared<Environment>(env);
        new_env->makeMove(move);

        node->addChild(std::make_unique<Node>(node, std::move(new_env), move, policy[move].item<float>()));
    }

    return value;
}

void MCTS::backpropagate(Node * leaf, float result)
{
    ePlayer player = leaf->getEnvironment()->getCurrentPlayer();
    // backpropagate the result to the root
    Node * current = leaf;
    while (current != nullptr)
    {
        current->incrementVisit();
        float value = current->getValue();
        if (current->getEnvironment()->getCurrentPlayer() == player)
        {
            value += result;
        }
        else
        {
            value -= result;
        }
        current->setValue(value);
        current = current->getParent();
    }
}

int MCTS::getBestMoveDeterministic() const
{
    // get move where visits is highest
    float                                      max_visits = 0;
    int                                        max_index  = 0;
    std::vector<std::unique_ptr<Node>> const & moves      = m_Root->getChildren();
    for (int i = 0; i < (int)moves.size(); i++)
    {
        if (moves[i]->getVisits() > max_visits)
        {
            max_visits = moves[i]->getValue();
            max_index  = i;
        }
    }
    return max_index;
}

int MCTS::getBestMoveStochastic() const
{
    std::vector<std::unique_ptr<Node>> const & children = m_Root->getChildren();
    std::vector<int>                           moves;
    for (auto const & node: children)
    {
        moves.push_back(node->getVisits());
    }
    // create a discrete distribution to pick from
    std::discrete_distribution<int> distribution(moves.begin(), moves.end());
    int                             index = distribution(g_Generator);
    return children.at(index)->getMove();
}

int MCTS::getTreeDepth(std::unique_ptr<Node> const & root)
{
    // recursive function of getting height of the tree
    if (root->getChildren().empty())
    {
        return 0;
    }
    int max_depth = 0;
    for (int i = 0; i < (int)root->getChildren().size(); i++)
    {
        int depth = MCTS::getTreeDepth(root->getChildren().at(i));
        if (depth > max_depth)
        {
            max_depth = depth;
        }
    }
    return max_depth + 1;
}
