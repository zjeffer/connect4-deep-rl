#include "game.hpp"

Game::Game(Settings* settings){
	m_Settings = settings;
	this->m_Env = new Environment(m_Settings->getRows(), m_Settings->getCols());
}

Game::~Game() {
	delete this->m_Env;
}

ePlayer Game::playGame() {
	Environment env = Environment(6, 7);
	ePlayer winner = ePlayer::NONE;

	while (!env.hasValidMoves() && g_running) {
		env.print();

		if(!this->playMove()) {
			break;
		}
	}

	winner = env.getWinner();
	env.print();
	LOG(INFO) << "Winner: " << static_cast<int>(winner);

	// TODO: save game to memory file

	return winner;
}

bool Game::playMove() {
	// play move and return if succesfully played



	// TODO: MCTS

	// TODO: get best move from mcts tree

	// TODO: make the move
	
}
