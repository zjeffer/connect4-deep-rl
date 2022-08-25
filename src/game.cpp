#include "game.hpp"

Game::Game(SelfPlaySettings* selfPlaySettings)
	: m_Settings(selfPlaySettings), m_Env(std::make_shared<Environment>(m_Settings->getRows(), m_Settings->getCols())) {

	std::shared_ptr<NeuralNetwork> nn = std::make_shared<NeuralNetwork>(m_Settings);
	for (auto& agentData : m_Settings->getAgents()) {
		m_Agents.push_back(new Agent(agentData.name, nn, m_Settings));
	}

	// create a random id
	std::string current_date = std::to_string(std::time(nullptr));
	m_GameID = "game-" + current_date + "-" + std::to_string(g_UniformIntDist(g_Generator));
}

Game::~Game() {
	// std::cout << "Game destructor" << std::endl;
	for (auto& agent : m_Agents) {
		delete agent;
	}
}

std::shared_ptr<Environment> Game::getEnvironment() const { return m_Env; }

ePlayer Game::playGame() {
	ePlayer winner = ePlayer::NONE;

	m_Env->print();
	while (m_Env->hasValidMoves() && g_Running) {
		if (playMove()) {
			// break if no moves left or someone won
			break;
		}
	}

	m_Env->togglePlayer();
	winner = m_Env->getWinner();

	if (!g_Running) {
		LOG(INFO) << "Game stopped";
		exit(EXIT_SUCCESS);
	}

	if (m_Settings->saveMemory()) {
		updateMemoryWithWinner(winner);
		if (!saveMemoryToFile()) {
			LOG(FATAL) << "Could not save memory to file";
			exit(EXIT_FAILURE);
		}
	}

	return winner;
}

bool Game::playMove() {
	// play move and return if game is over

	// get agent
	Agent* agent = m_Agents.at(static_cast<int>(m_Env->getCurrentPlayer()) - 1);

	// TODO: get new node from old tree
	Node* currentNode = new Node(m_Env);
	agent->getMCTS()->setRoot(currentNode);

	agent->getMCTS()->run_simulations();
	// calculate average action-value of all actions in the root node
	/* float value = 0.0f;
	for (auto &node : agent->getMCTS()->getRoot()->getChildren()) {
		float weight = (float)node->getVisits() / (float)agent->getMCTS()->getRoot()->getVisits();
		value += node->getQ() * weight;
	}
	LOG(INFO) << "Average action-value according to current player (" << agent->getName() << "): " << value; */

	// get best move from mcts tree
	int bestMove = -1;
	if (m_Settings->isStochastic()) {
		bestMove = agent->getMCTS()->getBestMoveStochastic();
	} else {
		bestMove = agent->getMCTS()->getBestMoveDeterministic();
	}

	// print moves and their q + u values
	const std::vector<std::unique_ptr<Node>>& children = agent->getMCTS()->getRoot()->getChildren();
	std::vector<float> moveProbs = std::vector<float>(m_Env->getCols(), 0.0f);
	for (auto& child : children) {
		moveProbs[child->getMove()] = (float)child->getVisits() / (float)agent->getMCTS()->getRoot()->getVisits();
		if (m_Settings->showMoves()) {
			LOG(DEBUG) << "Move: " << child->getMove() << " Q: " << child->getQ() << " U: " << child->getU()
					   << ". Visits: " << child->getVisits();
		}
	}

	if (m_Settings->saveMemory()) {
		// create memory element
		MemoryElement element;
		element.board = utils::boardToVector(m_Env->getBoard());
		element.currentPlayer = static_cast<uint8_t>(m_Env->getCurrentPlayer());
		element.moveList = moveProbs;
		element.winner = 0;
		// save element to memory
		addElementToMemory(element);

		if ((int)element.board.size() != m_Env->getRows() * m_Env->getCols()) {
			LOG(FATAL) << "Memory element has no board";
			exit(EXIT_FAILURE);
		}
	}

	LOG(INFO) << "Playing best move: " << bestMove;

	// make the best move
	m_Env->makeMove(bestMove);
	m_Env->print();

	return !m_Env->hasValidMoves() || m_Env->currentPlayerHasConnected4();
}

void Game::updateMemoryWithWinner(ePlayer winner) {
	// update memory with winner
	for (MemoryElement& element : m_Memory) {
		element.winner = static_cast<uint8_t>(winner);
	}
}

bool Game::saveMemoryToFile() {
	std::string folder = m_Settings->getMemoryFolder();
	if (!std::filesystem::exists(folder)) {
		std::filesystem::create_directory(folder);
	}
	std::string filename = folder + m_GameID + ".bin";
	return utils::writeMemoryElementsToFile(m_Memory, filename);
}

void Game::addElementToMemory(MemoryElement element) { m_Memory.push_back(element); }