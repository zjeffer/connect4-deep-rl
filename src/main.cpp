#include <cstdlib>
#include <exception>
#include <filesystem>
#include <stdexcept>
#include <torch/jit.h>
#include <torch/nn.h>
#include <torch/script.h>
#include <torch/torch.h>

#include <iostream>
#include <signal.h>

#include "game.hpp"
#include "train.hpp"
#include "utils/selfPlaySettings.hpp"
#include "utils/test.hpp"
#include "utils/trainerSettings.hpp"
#include "common.hpp"
#include "utils/types.hpp"

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
		return std::find(this->tokens.begin(), this->tokens.end(), option) !=
			   this->tokens.end();
	}

  private:
	std::vector<std::string> tokens;
};

[[noreturn]] void printUsage(char* filename) {
	// print help and exit
	std::cout << "Usage: " << filename << " [options]" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "  -h, --help\t\t\tPrint this help message" << std::endl;
	std::cout << "  --sims\t\t\tAmount of simulations" << std::endl;
	std::cout << "  --train\t\t\tTrain a new network" << std::endl;
	std::cout << "  --pipeline\t\tRepeatedly run games and retrain the network" << std::endl;
	std::cout << "  --model\t\t\tPath to model to use for selfplay or training" << std::endl;
	std::cout << "  --lr\t\t\t\tLearning rate" << std::endl;
	std::cout << "  --bs\t\t\t\tBatch size" << std::endl;
	exit(EXIT_SUCCESS);
}

void parseSelfPlayOptions(InputParser* inputParser, SelfPlaySettings *settings) {
	if (inputParser->cmdOptionExists("--model")) {
		settings->setModelPath(inputParser->getCmdOption("--model"));
	}

	// create/load model
	std::string modelPath = settings->getModelPath();
	if (!std::filesystem::exists(modelPath)) {
		NeuralNetwork *nn = new NeuralNetwork(settings);
		nn->saveModel(modelPath);
		delete nn;
	}

	// FOR DEBUGGING PURPOSES
	// settings->setShowMoves(true);

	// add agents
	settings->addAgent("Yellow", modelPath, ePlayer::YELLOW);
	settings->addAgent("Red", modelPath, ePlayer::RED);

	if (inputParser->cmdOptionExists("--sims")) {
		try {
			int sims = std::stoi(inputParser->getCmdOption("--sims"));
			settings->setSimulations(sims);
			if (sims < 1) {
				throw std::invalid_argument(
					"Amount must be greater or equal "
					"to 1");
			}
		} catch (std::invalid_argument &e) {
			std::cerr << "Invalid argument for --sims: " << e.what()
						<< std::endl;
			exit(EXIT_FAILURE);
		}
	}

	// set memory folder
	settings->setMemoryFolder("memory");
	try {
		if (inputParser->cmdOptionExists("--memory-folder")) {
			settings->setMemoryFolder(inputParser->getCmdOption("--memory-folder"));
		}
	} catch (const std::exception &e) {
		LOG(FATAL) << "Invalid memory folder: " << e.what();
		exit(EXIT_FAILURE);
	}
}

void runGame(SelfPlaySettings* settings, SelfPlayTally* tally) {
	Game game = Game(settings);
	ePlayer winner = game.playGame();
	if (winner == ePlayer::NONE) {
		tally->draws++;
	} else if (winner == ePlayer::YELLOW) {
		tally->yellow++;
	} else if (winner == ePlayer::RED) {
		tally->red++;
	}
	LOG(INFO) << "Winner: " << static_cast<int>(winner) << "\n";
	LOG(INFO) << "========= Tally:";
	LOG(INFO) << "===== Yellow: " << tally->yellow;
	LOG(INFO) << "===== Red: " << tally->red;
	LOG(INFO) << "===== Draws: " << tally->draws;
	LOG(INFO) << "===============  \n";
	LOG(INFO) << "\n\n\n";
}

void parseTrainingOptions(InputParser* inputParser, TrainerSettings* settings) {
	if (inputParser->cmdOptionExists("--model")) {
		std::string modelPath = inputParser->getCmdOption("--model");
		if (std::filesystem::is_regular_file(modelPath) && modelPath.ends_with(".pt")) {
			settings->setModelPath(modelPath);
		} else {
			LOG(FATAL) << "Model file invalid: " << modelPath;
			exit(EXIT_FAILURE);
		}
	}

	try {
		if (inputParser->cmdOptionExists("--bs")) {
			settings->setBatchSize(std::stoi(inputParser->getCmdOption("--bs")));
		}
	} catch (const std::invalid_argument &e) {
		LOG(FATAL) << "Invalid batch size: " << e.what();
		exit(EXIT_FAILURE);
	}

	try {
		if (inputParser->cmdOptionExists("--lr")) {
			settings->setLearningRate(std::stod(inputParser->getCmdOption("--lr")));
		}
	} catch (const std::invalid_argument &e) {
		LOG(FATAL) << "Invalid learning rate: " << e.what();
		exit(EXIT_FAILURE);
	}
	

	try {
		if (inputParser->cmdOptionExists("--memory-folder")) {
			settings->setMemoryFolder(inputParser->getCmdOption("--memory-folder"));
		}
	} catch (const std::exception &e) {
		LOG(FATAL) << "Invalid memory folder: " << e.what();
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[]) {
	// signal handling
	signal(SIGINT, signal_handling);
	signal(SIGTERM, signal_handling);

	// parse arguments
	InputParser inputParser = InputParser(argc, argv);
	if (inputParser.cmdOptionExists("-h") ||
		inputParser.cmdOptionExists("--help")) {
		printUsage(argv[0]);
	}

	// create logger
	logger = std::make_shared<Logger>();
	LOG(DEBUG) << "Logger initialized!";

	// set random seed
	g_generator.seed(std::random_device{}());
	// LOG(DEBUG) << "Test random value: " << g_generator();

	// test
	if (inputParser.cmdOptionExists("--test")) {
		Test::testHorizontalWin();
		Test::testVerticalWin();
		Test::testDiagonalWin();
		Test::testEasyPuzzle();
		// testStochasticDistribution();
		exit(EXIT_SUCCESS);
	}

	// TODO: load all settings from a json file or something
	if (inputParser.cmdOptionExists("--train")) {
		// parse settings
		TrainerSettings trainerSettings = TrainerSettings();
		parseTrainingOptions(&inputParser, &trainerSettings);
		// create trainer and train
		Trainer trainer = Trainer(&trainerSettings);
		trainer.train();
		return 0;
	} else { // selfplay
		SelfPlaySettings selfPlaySettings = SelfPlaySettings();
		parseSelfPlayOptions(&inputParser, &selfPlaySettings);

		if (inputParser.cmdOptionExists("--pipeline")) {
			// run full pipeline with selfplay & training
			LOG(INFO) << "Running full pipeline with 50 games...";
			TrainerSettings trainerSettings = TrainerSettings();
			parseTrainingOptions(&inputParser, &trainerSettings);
			
			while (g_running) {	
				LOG(INFO) << "Running selfplay...";
				SelfPlayTally tally = SelfPlayTally();
				for (int gameCount = 1; gameCount <= 50; gameCount++) {
					LOG(INFO) << "\n\n\tStarting game " << gameCount << "...\n";
					runGame(&selfPlaySettings, &tally);
				}
				LOG(INFO) << "Training new model...";
				// train with these games
				
				Trainer trainer = Trainer(&trainerSettings);
				std::filesystem::path trainedModelName = trainer.train();

				// move memory to old/ folder
				LOG(INFO) << "Moving old games...";
				std::filesystem::path memoryFolder = selfPlaySettings.getMemoryFolder();
				std::filesystem::path oldFolder = std::filesystem::path(memoryFolder).append("old");
				assert(memoryFolder.string() != oldFolder.string());
				if (!std::filesystem::exists(oldFolder)) {
					std::filesystem::create_directory(oldFolder);
				}
				for (auto& p : std::filesystem::directory_iterator(memoryFolder)) {
					if (p.path().extension() == ".bin") {
						// move to old folder
						std::filesystem::path newPath = oldFolder / p.path().filename().string();
						std::filesystem::rename(p.path(), newPath);
					}
				}

				// TODO: evaluate with older model
				// LOG(INFO) << "Evaluating against old model...";
				// TODO: keep model with best performance

				// move old model to old/ folder
				LOG(INFO) << "Moving old model...";
				std::filesystem::path modelPath = std::filesystem::path(selfPlaySettings.getModelPath());
				if (modelPath.string().starts_with("./")) {
					modelPath = modelPath.string().substr(2);
				}
				std::filesystem::path modelFolder = modelPath.parent_path();
				std::filesystem::path oldModelPath = std::filesystem::path(modelFolder).append("old").append(modelPath.filename().string());
				if (!std::filesystem::exists(oldModelPath.parent_path())) {
					std::filesystem::create_directory(oldModelPath.parent_path());
				}
				LOG(INFO) << "Moving " << modelPath << " to " << oldModelPath;
				std::filesystem::rename(modelPath, oldModelPath);

				LOG(INFO) << "Setting new model path to " << trainedModelName;
				trainerSettings.setModelPath(trainedModelName);
				selfPlaySettings.setModelPath(trainedModelName);
				// save new model
				LOG(INFO) << "Saving new model...";
				
			}
		} else {
			// run games infinitely
			SelfPlayTally tally;
			while (g_running) {
				runGame(&selfPlaySettings, &tally);
			}
		}
	}

	return 0;
}