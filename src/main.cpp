#include <torch/torch.h>
#include <torch/jit.h>
#include <torch/nn.h>
#include <torch/script.h>

#include <iostream>
#include <signal.h>

#include "connect4/environment.hpp"
#include "test.hpp"
#include "settings.hpp"
#include "game.hpp"
#include "common.hpp"


void signal_handling(int signal) {
	std::cerr << "Signal " << signal << " received. Quitting..." << std::endl;
	g_running = false;
}


int main(int argc, char* argv[]){
	// signal handling
	signal(SIGINT, signal_handling);
	signal(SIGTERM, signal_handling);


	// create logger
	logger = std::make_shared<Logger>();
	LOG(DEBUG) << "Logger initialized!";

	// TODO: load settings from file
	Settings settings = Settings();
	settings.setCols(7);
	settings.setRows(6);
	settings.setSimulations(400);
	settings.setStochastic(true);

	// create model
	NeuralNetwork* nn = new NeuralNetwork(&settings);
	nn->saveModel("models/model.pt");
	delete nn;

	// add agents
	settings.addAgent("Yellow", "models/model.pt", ePlayer::YELLOW);
	settings.addAgent("Red", "models/model.pt", ePlayer::RED);

	// create game
	Game game = Game(&settings);
	
	ePlayer winner = game.playGame();
	LOG(INFO) << "Winner: " << static_cast<int>(winner);

	return 0;
}