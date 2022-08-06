#include "game.hpp"
#include "mcts.hpp"

Game::Game(Settings* settings){
	m_Settings = settings;
	this->m_Env = new Environment(m_Settings->getRows(), m_Settings->getCols());
	this->m_Agents = std::vector<Agent*>();
	for (auto &agentData : m_Settings->getAgents()){
		this->m_Agents.push_back(new Agent(agentData.name, agentData.nn_path, m_Settings));
	}
}

Game::~Game() {
	delete this->m_Env;
}

ePlayer Game::playGame() {
	ePlayer winner = ePlayer::NONE;

	this->m_Env->newEnvironment(m_Settings->getRows(), m_Settings->getCols());

	while (m_Env->hasValidMoves() && g_running) {
		m_Env->print();

		if(!this->playMove()) {
			break;
		}
	}

	winner = m_Env->getWinner();
	m_Env->print();
	LOG(INFO) << "Winner: " << static_cast<int>(winner);

	// TODO: save game to memory file

	return winner;
}

bool Game::playMove() {
	// play move and return if succesfully played

	// get agent
	Agent* agent = this->m_Agents.at(static_cast<int>(m_Env->getCurrentPlayer()) - 1);

	agent->getMCTS()->run_simulations();

	// get best move from mcts tree
	int bestMove = agent->getMCTS()->getBestMoveDeterministic();

	// make the best move
	return this->m_Env->makeMove(bestMove);
}
