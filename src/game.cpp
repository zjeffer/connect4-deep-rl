#include "game.hpp"
#include "mcts.hpp"
#include <ATen/ops/from_blob.h>
#include <cstdint>

Game::Game(Settings* settings){
	m_Settings = settings;
	m_Env = new Environment(m_Settings->getRows(), m_Settings->getCols());
	m_Agents = std::vector<Agent*>();
	for (auto &agentData : m_Settings->getAgents()){
		m_Agents.push_back(new Agent(agentData.name, agentData.nn_path, m_Settings));
	}

	
	// for debugging purposes: try pre-configured board
	std::vector<uint8_t> data = {
		0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0,
		0, 2, 2, 2, 0, 0, 0,
		0, 1, 1, 1, 0, 0, 0,
	};
	auto options = torch::TensorOptions().dtype(torch::kInt8);
	torch::Tensor board = torch::from_blob(data.data(), {m_Settings->getRows(), m_Settings->getCols()}, options);
	m_Env->setBoard(board); 
	
}

Game::~Game() {
	std::cout << "Game destructor" << std::endl;
	delete m_Env;
	for (auto &agent : m_Agents){
		delete agent;
	}
}

ePlayer Game::playGame() {
	ePlayer winner = ePlayer::NONE;

	m_Env->print();
	while (m_Env->hasValidMoves() && g_running) {
		if(this->playMove()) {
			// break if no moves left or someone won
			break;
		}
	}

	winner = m_Env->getWinner();

	// TODO: save game to memory file

	return winner;
}

bool Game::playMove() {
	// play move and return if game is over

	// get agent
	Agent* agent = this->m_Agents.at(static_cast<int>(m_Env->getCurrentPlayer()) - 1);

	// TODO: get new node from old tree
	Node* currentNode = new Node(m_Env);
	agent->getMCTS()->setRoot(currentNode);

	agent->getMCTS()->run_simulations();

	// get best move from mcts tree
	int bestMove = agent->getMCTS()->getBestMoveStochastic();

	// print moves and their q + u values
	std::vector<Node*> children = agent->getMCTS()->getRoot()->getChildren();
	for (Node* child : children) {
		LOG(DEBUG) << "Move: " << child->getMove() << " Q: " << child->getQ() << " U: " << child->getU() << ". Visits: " << child->getVisits();
	}

	LOG(INFO) << "Playing best move: " << bestMove;

	// make the best move
	this->m_Env->makeMove(bestMove);
	m_Env->print();

	return !m_Env->hasValidMoves() || m_Env->currentPlayerHasConnected4();
}
