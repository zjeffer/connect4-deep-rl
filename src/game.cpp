#include "game.hpp"
#include "common.hpp"
#include "mcts.hpp"
#include "utils/types.hpp"
#include "utils/utils.hpp"
#include <ATen/ops/from_blob.h>
#include <cstdint>
#include <cstdlib>
#include <filesystem>

Game::Game(SelfPlaySettings* selfPlaySettings){
	m_Settings = selfPlaySettings;
	m_Env = new Environment(m_Settings->getRows(), m_Settings->getCols());
	m_Agents = std::vector<Agent*>();
	for (auto &agentData : m_Settings->getAgents()){
		m_Agents.push_back(new Agent(agentData.name, agentData.nn_path, m_Settings));
	}

	/* 
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
	*/ 

	// create a random id
	std::string current_date = std::to_string(std::time(nullptr));
	m_GameID = "game-" + current_date + "-" + std::to_string(g_uniform_int_dist(g_generator));
}

Game::~Game() {
	// std::cout << "Game destructor" << std::endl;
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

	m_Env->togglePlayer();
	winner = m_Env->getWinner();

	updateMemoryWithWinner(winner);

	if (!saveMemoryToFile()){
		LOG(FATAL) << "Could not save memory to file";
		exit(EXIT_FAILURE);
	}

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
	std::vector<MoveProb> moveProbs = std::vector<MoveProb>();
	for (Node* child : children) {
		MoveProb moveProb = MoveProb{child->getMove(), child->getQ() + child->getU()};
		moveProbs.push_back(moveProb);
		// LOG(DEBUG) << "Move: " << child->getMove() << " Q: " << child->getQ() << " U: " << child->getU() << ". Visits: " << child->getVisits();
	}

	// create memory element
	MemoryElement element;
	element.board = utils::boardToVector(m_Env->getBoard());
	element.moves = moveProbs;
	element.winner= 0;

	// save element to memory
	this->addElementToMemory(element);

	LOG(INFO) << "Playing best move: " << bestMove;

	// make the best move
	this->m_Env->makeMove(bestMove);
	m_Env->print();

	return !m_Env->hasValidMoves() || m_Env->currentPlayerHasConnected4();
}


void Game::updateMemoryWithWinner(ePlayer winner) {
	// update memory with winner
	for (MemoryElement &element : this->memory) {
		element.winner = static_cast<int>(winner);
	}
}

bool Game::saveMemoryToFile(){
	std::string folder = m_Settings->getMemoryFolder();
	if (!std::filesystem::exists(folder)) {
		std::filesystem::create_directory(folder);
	}
	std::string filename = folder + m_GameID + ".bin";

	std::ofstream file(filename, std::ios::binary);
	if (file.is_open()) {
		for (MemoryElement &element : this->memory) {
			file.write(reinterpret_cast<char*>(&element), sizeof(element));
		}
		file.close();
		return true;
	}
	return false;
}

void Game::addElementToMemory(MemoryElement element) {
	this->memory.push_back(element);
}