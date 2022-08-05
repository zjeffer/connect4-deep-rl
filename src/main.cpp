#include <torch/torch.h>
#include <torch/jit.h>
#include <torch/nn.h>
#include <torch/script.h>

#include <iostream>
#include <signal.h>

#include "connect4/environment.hpp"
#include "test.hpp"
#include "common.hpp"


void signal_handling(int signal) {
	std::cerr << "Signal " << signal << " received. Quitting..." << std::endl;
	g_running = false;
}


int main(int argc, char* argv[]){
	// create logger
	logger = std::make_shared<Logger>();
	LOG(DEBUG) << "Logger initialized!";

	testHorizontalWin();

	testVerticalWin();

	testDiagonalWin();

	return 0;
}