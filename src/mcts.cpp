#include "mcts.hpp"

MCTS::MCTS(SelfPlaySettings* selfPlaySettings, std::shared_ptr<Node> root, std::shared_ptr<NeuralNetwork> const& nn)
  : m_Settings(selfPlaySettings)
  , m_NN(nn)
{

    if (root == nullptr)
    {
        root = std::make_shared<Node>(std::make_shared<Environment>(selfPlaySettings->getRows(), selfPlaySettings->getCols()));
    }
    setRoot(root);

    // uses torch::kCPU if useCUDA is false
    if (m_Settings->useCUDA())
    {
        m_Device = torch::kCUDA;
    }
}

MCTS::~MCTS() {}

std::shared_ptr<Node> const& MCTS::getRoot() const
{
    return m_Root;
}

void MCTS::setRoot(std::shared_ptr<Node> root)
{
    m_Root = root;
    m_Root->setParent(nullptr);
}

void MCTS::run_simulations()
{
    std::shared_ptr<Node> root = getRoot();

    int sims = m_Settings->getSimulations();
    LINFO << "Running " << sims << " simulations...\n";
    tqdm bar;
    for (int i = 0; i < sims && g_Running; i++)
    {
        bar.progress(i, sims);
        // step 1: selection
        std::shared_ptr<Node> selected = select(root);
        // step 2 and 3: expansion and evaluation
        float result = expand(selected);
        // step 4: backpropagation
        backpropagate(selected, result);
    }
    // std::cout << "Tree depth: " << MCTS::getTreeDepth(root);
    std::cout << std::endl;
}

std::shared_ptr<Node> MCTS::select(std::shared_ptr<Node> root)
{
    // keep selecting nodes using the Q+U formula
    // until we reach a node not yet expanded
    std::shared_ptr<Node> current = root;
    while (current->getChildren().size() > 0)
    {
        std::shared_ptr<Node> best_child = std::shared_ptr<Node>(nullptr);
        float                 best_score = -2;
        for (auto& child: current->getChildren())
        {
            float score = child->getQ() + child->getU();
            if (score > best_score)
            {
                best_child = child;
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

float MCTS::expand(std::shared_ptr<Node> node)
{
    // expand the node by adding a child for each possible move
    std::shared_ptr<Environment> env = node->getEnvironment();

    torch::Tensor                           input  = m_NN->boardToInput(env);
    std::pair<torch::Tensor, torch::Tensor> output = m_NN->predict(input);

    // policy output
    torch::Tensor policy = output.first.view({7});
    // value output (= step 3: evaluation)
    float value = output.second.item<float>();

    if (node->getParent() == nullptr)
    {
        // root node, add dirichlet noise to policy
        auto  noise = utils::calculateDirichletNoise(policy);
        float frac  = 0.25;
        for (int i = 0; i < policy.size(0); i++)
        {
            // LWARN << policy[i].item<float>() << "--" << noise[i] << " ==> " << policy[i].item<float>() * (1 - frac) + noise[i] * frac;
            policy[i] = policy[i] * (1 - frac) + noise[i] * frac;
        }
    }

    std::vector<int> valid_moves = env->getValidMoves();
    if (valid_moves.size() == 0)
    {
        float win =  env->getWinner() == ePlayer::NONE ? 0 : 1;
        return win;
    }

    if (env->getWinner() != ePlayer::NONE)
    {
        return 1;
    }

    // add a child node to the leaf node for every possible action (= step 2: expansion)
    for (auto const& move: valid_moves)
    {
        // copy the environment and make the new move
        std::shared_ptr<Environment> new_env = std::make_shared<Environment>(env);
        new_env->makeMove(move);

        node->addChild(std::make_shared<Node>(node, std::move(new_env), move, policy[move].item<float>()));
    }

    return value;
}

void MCTS::backpropagate(std::shared_ptr<Node> leaf, float result)
{
    ePlayer player = leaf->getEnvironment()->getCurrentPlayer();
    // backpropagate the result to the root
    std::shared_ptr<Node> current = leaf;
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
    float                                     max_visits = 0;
    int                                       max_index  = 0;
    std::vector<std::shared_ptr<Node>> const& moves      = m_Root->getChildren();
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
    std::vector<std::shared_ptr<Node>> const& children = m_Root->getChildren();
    std::vector<int>                          moves;
    for (auto const& node: children)
    {
        moves.push_back(node->getVisits());
    }
    // create a discrete distribution to pick from
    std::discrete_distribution<int> distribution(moves.begin(), moves.end());
    int                             index = distribution(g_Generator);
    return children.at(index)->getMove();
}

int MCTS::getTreeDepth(Node* root)
{
    // recursive function of getting height of the tree
    if (root->getChildren().empty())
    {
        return 0;
    }
    int max_depth = 0;
    for (int i = 0; i < (int)root->getChildren().size(); i++)
    {
        int depth = MCTS::getTreeDepth(root->getChildren().at(i).get());
        if (depth > max_depth)
        {
            max_depth = depth;
        }
    }
    return max_depth + 1;
}
