#include <filesystem>
#include <torch/torch.h>
#include <torch/jit.h>
#include <torch/nn.h>
#include <torch/script.h>

#include <iostream>
#include <signal.h>

#include "connect4/environment.hpp"
#include "utils/test.hpp"
#include "utils/selfPlaySettings.hpp"
#include "game.hpp"
#include "common.hpp"


void signal_handling(int signal) {
	std::cerr << "Signal " << signal << " received. Quitting..." << std::endl;
	g_running = false;
}

// argument parsing (https://stackoverflow.com/a/868894/10180569)
class InputParser {
  public:
    InputParser(int &argc, char **argv) {
        for (int i = 1; i < argc; ++i) {
            this->tokens.push_back(std::string(argv[i]));
        }
    }

    /// @author iain
    const std::string &getCmdOption(const std::string &option) const {
        std::vector<std::string>::const_iterator itr;
        itr = std::find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end() && ++itr != this->tokens.end()) {
            return *itr;
        }
        static const std::string empty_string("");
        return empty_string;
    }

    /// @author iain
    bool cmdOptionExists(const std::string &option) const {
        return std::find(this->tokens.begin(), this->tokens.end(), option) != this->tokens.end();
    }

  private:
    std::vector<std::string> tokens;
};

void printUsage(char* filename) {
	// print help and exit
	std::cout << "Usage: " << filename << " [options]" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "  -h, --help\t\t\tPrint this help message" << std::endl;
	std::cout << "  --sims\t\t\tAmount of simulations" << std::endl;
	exit(EXIT_SUCCESS);
}


int main(int argc, char* argv[]){
	// signal handling
	signal(SIGINT, signal_handling);
	signal(SIGTERM, signal_handling);


	// create logger
	logger = std::make_shared<Logger>();
	LOG(DEBUG) << "Logger initialized!";

	// set random seed
	g_generator.seed(std::random_device{}());
	// LOG(DEBUG) << "Test random value: " << g_generator();

	// TODO: load selfPlaySettings from file
	SelfPlaySettings selfPlaySettings = SelfPlaySettings();
	selfPlaySettings.setCols(7);
	selfPlaySettings.setRows(6);
	selfPlaySettings.setStochastic(true);

	// create model
	if (!std::filesystem::exists("models/model.pt")){
		NeuralNetwork* nn = new NeuralNetwork(&selfPlaySettings);
		nn->saveModel("models/model.pt");
		delete nn;
	}

	// add agents
	selfPlaySettings.addAgent("Yellow", "models/model.pt", ePlayer::YELLOW);
	selfPlaySettings.addAgent("Red", "models/model.pt", ePlayer::RED);

	// parse arguments
	InputParser inputParser = InputParser(argc, argv);
	if (inputParser.cmdOptionExists("-h") || inputParser.cmdOptionExists("--help")) {
		printUsage(argv[0]);
	}

	if (inputParser.cmdOptionExists("--sims")) {
		try {
			int sims = std::stoi(inputParser.getCmdOption("--sims"));
			selfPlaySettings.setSimulations(sims);
			if (sims < 1) {
				throw std::invalid_argument("Amount must be greater or equal to 1");
			}
		} catch (std::invalid_argument& e) {
			std::cerr << "Invalid argument for --sims: " << e.what() << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	int draws = 0, yellow = 0, red = 0;

	while(g_running) {
		// create game
		Game game = Game(&selfPlaySettings);
		ePlayer winner = game.playGame();
		if (winner == ePlayer::NONE) {
			draws++;
		} else if (winner == ePlayer::YELLOW) {
			yellow++;
		} else if (winner == ePlayer::RED) {
			red++;
		}
		LOG(INFO) << "Winner: " << static_cast<int>(winner) << "\n";
		LOG(INFO) << "========= Tally:";
		LOG(INFO) << "===== Yellow: " << yellow;
		LOG(INFO) << "===== Red: " << red;
		LOG(INFO) << "===== Draws: " << draws;
		LOG(INFO) << "===============  \n";
		LOG(INFO) << "\n\n\n";
	}

	

	return 0;
}