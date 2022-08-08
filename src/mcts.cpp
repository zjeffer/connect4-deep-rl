#include "mcts.hpp"
#include "common.hpp"
#include "utils/tqdm.h"

MCTS::MCTS(SelfPlaySettings *selfPlaySettings, Node *root,
		   const std::shared_ptr<NeuralNetwork> &nn) {
	m_Settings = selfPlaySettings;
	m_NN = nn;

	if (root == nullptr) {
		Environment *env =
			new Environment(selfPlaySettings->getRows(), selfPlaySettings->getCols());
		root = new Node(env);
	}
	m_Root = root;

	if (m_Settings->useGPU()) {
		m_Device = torch::kCUDA;
	}
}

MCTS::~MCTS() {
	// delete m_Root;
}

Node *MCTS::getRoot() { return m_Root; }

void MCTS::setRoot(Node *root) { m_Root = root; }

void MCTS::run_simulations() {
	Node *root = this->getRoot();

	tqdm bar;
	int sims = m_Settings->getSimulations();
	LOG(INFO) << "Running " << sims << " simulations...\n";
	for (int i = 0; i < sims && g_running; i++) {
		bar.progress(i, sims);
		Node *selected = this->select(root);
		float result = this->expand(selected);
		this->backpropagate(selected, result);
	}
	std::cout << std::endl;
}

Node *MCTS::select(Node *root) {
	// keep selecting nodes using the Q+U formula
	// until we reach a node not yet expanded
	Node *current = root;
	// TODO: use boolean instead of checking size of vector
	while (current->getChildren().size() != 0) {
		std::vector<Node *> children = current->getChildren();
		Node *best_child = nullptr;
		float best_score = -1;
		for (Node *child : children) {
			float score = child->getQ() + child->getU();
			if (score > best_score) {
				best_child = child;
				best_score = score;
			}
		}
		if (best_child == nullptr) {
			LOG(FATAL) << "Error: best child is null";
			exit(EXIT_FAILURE);
		}
		current = best_child;
	}
	return current;
}

float MCTS::expand(Node *node) {
	// expand the node by adding a child for each possible move
	Environment *env = node->getEnvironment();

	torch::Tensor input = m_NN->boardToInput(env);
	std::tuple<torch::Tensor, torch::Tensor> output = m_NN->predict(input);

	// policy
	torch::Tensor policy = std::get<0>(output).view({7});
	// value
	float value = std::get<1>(output).item<float>();

	std::vector<int> valid_moves = env->getValidMoves();

	if (valid_moves.size() == 0) {
		return env->getWinner() == ePlayer::NONE ? 0 : 1;
	}

	if (env->getWinner() != ePlayer::NONE) {
		return 1;
	}

	for (const auto& move : valid_moves) {
		// create the new environment
		Environment* new_env = new Environment(env->getBoard().detach().clone(), env->getCurrentPlayer());
		new_env->makeMove(move);

		Node *child = new Node(node, new_env, move, policy[move].item<float>());
		node->addChild(child);
	}

	return value;
}

void MCTS::backpropagate(Node *root, float result) {
	ePlayer player = root->getEnvironment()->getCurrentPlayer();
	// backpropagate the result to the root
	Node *current = root;
	while (current != nullptr) {
		current->incrementVisit();
		float value = current->getValue();
		if (current->getEnvironment()->getCurrentPlayer() == player) {
			value += result;
		} else {
			value -= result;
		}
		current->setValue(value);
		current = current->getParent();
	}
}

int MCTS::getBestMoveDeterministic() {
	// get move where prob is highest
	float max_prob = 0;
	int max_index = 0;
	std::vector<Node *> moves = m_Root->getChildren();
	for (int i = 0; i < (int)moves.size(); i++) {
		if (moves[i]->getVisits() > max_prob) {
			max_prob = moves[i]->getValue();
			max_index = i;
		}
	}
	return max_index;
}

int MCTS::getBestMoveStochastic() {
	std::vector<Node *> children = m_Root->getChildren();
	std::vector<int> moves;
	for (const auto &node : children) {
		moves.push_back(node->getMove());
	}
	std::discrete_distribution<int> distribution(moves.begin(), moves.end());
	int index = distribution(g_generator);

	return children[index]->getMove();
}

int MCTS::getTreeDepth(Node *root) {
	int depth = 0;
	Node *current = root;
	while (current->getParent() != nullptr) {
		depth++;
		current = current->getParent();
	}
	return depth;
}
