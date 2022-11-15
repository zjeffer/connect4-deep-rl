#include <signal.h>

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

#include "common.hpp"
#include "game.hpp"
#include "train.hpp"
#include "utils/selfPlaySettings.hpp"
#include "utils/test.hpp"
#include "utils/trainerSettings.hpp"
#include "utils/types.hpp"

void signal_handling(int signal)
{
    std::cerr << "Signal " << signal << " received. Quitting..." << std::endl;
    g_Running = false;
}

// argument parsing (https://stackoverflow.com/a/868894/10180569)
class InputParser
{
  public:
    InputParser(int& argc, char** argv)
    {
        for (int i = 1; i < argc; ++i)
        {
            this->tokens.push_back(std::string(argv[i]));
        }
    }

    /// @author iain
    std::string const& getCmdOption(std::string const& option) const
    {
        std::vector<std::string>::const_iterator itr;
        itr = std::find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end() && ++itr != this->tokens.end())
        {
            return *itr;
        }
        static const std::string empty_string("");
        return empty_string;
    }

    /// @author iain
    bool cmdOptionExists(std::string const& option) const
    {
        return std::find(this->tokens.begin(), this->tokens.end(), option) != this->tokens.end();
    }

  private:
    std::vector<std::string> tokens;
};

[[noreturn]] void printUsage(std::string filename)
{
    // print help and exit
    std::cout << "Usage: " << filename << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help\t\tPrint this help message" << std::endl;
    std::cout << "  --sims\t\tAmount of simulations" << std::endl;
    std::cout << "  --memory-folder\tFolder to save the games to or load the dataset from" << std::endl;
    std::cout << "  --train\t\tTrain a new network" << std::endl;
    std::cout << "  --pipeline\t\tRepeatedly run games and retrain the network. Append a number to specify amount of "
                 "games per pipeline."
              << std::endl;
    std::cout << "  --model\t\tPath to model to use for selfplay or training" << std::endl;
    std::cout << "  --lr\t\t\tLearning rate" << std::endl;
    std::cout << "  --bs\t\t\tBatch size" << std::endl;
    std::cout << "  --test\t\tRun tests" << std::endl;
    exit(EXIT_SUCCESS);
}

void parseSelfPlayOptions(InputParser* inputParser, SelfPlaySettings* settings)
{
    // set AI model
    if (inputParser->cmdOptionExists("--model"))
    {
        settings->setModelPath(inputParser->getCmdOption("--model"));
    }

    // create/load model
    std::string modelPath = settings->getModelPath();
    if (!std::filesystem::exists(modelPath))
    {
        NeuralNetwork nn = NeuralNetwork(settings);
        nn.saveModel(modelPath);
    }

    // FOR DEBUGGING PURPOSES: show Q+U+visits for every possible action
    settings->setShowMoves(true);

    // add agents
    settings->addAgent("Yellow", modelPath, ePlayer::YELLOW);
    settings->addAgent("Red", modelPath, ePlayer::RED);

    // set #simulations per move
    if (inputParser->cmdOptionExists("--sims"))
    {
        try
        {
            int sims = std::stoi(inputParser->getCmdOption("--sims"));
            settings->setSimulations(sims);
            if (sims < 1)
            {
                throw std::invalid_argument(
                  "Amount must be greater or equal "
                  "to 1");
            }
        }
        catch (std::invalid_argument& e)
        {
            std::cerr << "Invalid argument for --sims: " << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    // set memory folder
    settings->setMemoryFolder("memory");
    try
    {
        if (inputParser->cmdOptionExists("--memory-folder"))
        {
            settings->setMemoryFolder(inputParser->getCmdOption("--memory-folder"));
        }
    }
    catch (std::exception const& e)
    {
        LFATAL << "Invalid memory folder: " << e.what();
        exit(EXIT_FAILURE);
    }

    // set amount of pipeline games
    if (inputParser->cmdOptionExists("--pipeline"))
    {
        std::string games = inputParser->getCmdOption("--pipeline");
        if (games.length() > 0 && !games.starts_with(("-")))
        {
            try
            {
                settings->setPipelineGames(std::stoi(games));
                LINFO << "Pipeline: " << settings->getPipelineGames() << " games.";
            }
            catch (std::invalid_argument const& e)
            {
                LWARN << "Invalid amount of games: " << e.what();
            }
        }
    }
}

void runGame(SelfPlaySettings* settings, SelfPlayTally* tally)
{
    Game    game   = Game(settings);
    ePlayer winner = game.playGame();
    if (winner == ePlayer::NONE)
    {
        tally->draws++;
    }
    else if (winner == ePlayer::YELLOW)
    {
        tally->yellow++;
    }
    else if (winner == ePlayer::RED)
    {
        tally->red++;
    }
    LINFO << "Winner: " << static_cast<int>(winner) << "\n";
    LINFO << "========= Tally:";
    LINFO << "===== Yellow: " << tally->yellow;
    LINFO << "===== Red: " << tally->red;
    LINFO << "===== Draws: " << tally->draws;
    LINFO << "===============  \n";
    LINFO << "\n\n\n";
}

void parseTrainingOptions(InputParser* inputParser, TrainerSettings* settings)
{
    if (inputParser->cmdOptionExists("--model"))
    {
        std::string modelPath = inputParser->getCmdOption("--model");
        if (std::filesystem::is_regular_file(modelPath) && modelPath.ends_with(".pt"))
        {
            settings->setModelPath(modelPath);
        }
        else
        {
            LFATAL << "Model file invalid: " << modelPath;
        }
    }

    try
    {
        if (inputParser->cmdOptionExists("--bs"))
        {
            settings->setBatchSize(std::stoi(inputParser->getCmdOption("--bs")));
        }
    }
    catch (std::invalid_argument const& e)
    {
        LFATAL << "Invalid batch size: " << e.what();
    }

    try
    {
        if (inputParser->cmdOptionExists("--lr"))
        {
            settings->setLearningRate(std::stod(inputParser->getCmdOption("--lr")));
        }
    }
    catch (std::invalid_argument const& e)
    {
        LFATAL << "Invalid learning rate: " << e.what();
    }

    try
    {
        if (inputParser->cmdOptionExists("--memory-folder"))
        {
            settings->setMemoryFolder(inputParser->getCmdOption("--memory-folder"));
        }
    }
    catch (std::exception const& e)
    {
        LFATAL << "Invalid memory folder: " << e.what();
    }

    try
    {
        if (inputParser->cmdOptionExists("--epochs"))
        {
            settings->setEpochs(std::stoi(inputParser->getCmdOption("--epochs")));
        }
    }
    catch (std::exception const& e)
    {
        LFATAL << "Invalid amount of epochs: " << e.what();
    }
}

int main(int argc, char* argv[])
{
    // signal handling
    signal(SIGINT, signal_handling);
    signal(SIGTERM, signal_handling);

    // parse arguments
    InputParser inputParser = InputParser(argc, argv);
    if (inputParser.cmdOptionExists("-h") || inputParser.cmdOptionExists("--help"))
    {
        printUsage(argv[0]);
    }

    // create logger
    g_Logger = std::make_shared<Logger>();
    LDEBUG << "Logger initialized!";

    // set random seed
    g_Generator.seed(std::random_device{}());
    // LDEBUG << "Test random value: " << g_Generator();

    // test
    if (inputParser.cmdOptionExists("--test"))
    {
        Test::testHorizontalWin();
        Test::testVerticalWin();
        Test::testDiagonalWin();
        Test::testEasyPuzzle();
        Test::testStochasticDistribution();
        Test::testReadAndWriteMemoryElement();
        exit(EXIT_SUCCESS);
    }

    // TODO: load all settings from a json file or something
    if (inputParser.cmdOptionExists("--train"))
    {
        // parse settings
        TrainerSettings trainerSettings = TrainerSettings();
        parseTrainingOptions(&inputParser, &trainerSettings);
        // create trainer and train
        Trainer trainer = Trainer(&trainerSettings);
        trainer.train();
        return 0;
    }
    else
    { // selfplay
        SelfPlaySettings selfPlaySettings = SelfPlaySettings();
        parseSelfPlayOptions(&inputParser, &selfPlaySettings);

        if (inputParser.cmdOptionExists("--pipeline"))
        {
            // run full pipeline with selfplay & training

            LINFO << "Running full pipeline with " << selfPlaySettings.getPipelineGames() << " games...";
            TrainerSettings trainerSettings = TrainerSettings();
            parseTrainingOptions(&inputParser, &trainerSettings);

            while (g_Running)
            {
                LINFO << "Running selfplay...";
                SelfPlayTally tally = SelfPlayTally();
                for (int gameCount = 1; gameCount <= selfPlaySettings.getPipelineGames(); gameCount++)
                {
                    LINFO << "\n\n\tStarting game " << gameCount << "...\n";
                    runGame(&selfPlaySettings, &tally);
                }
                LINFO << "Training new model...";
                // train with these games

                Trainer               trainer          = Trainer(&trainerSettings);
                std::filesystem::path trainedModelName = trainer.train();

                // move memory to old/ folder
                LINFO << "Moving old games...";
                std::filesystem::path memoryFolder = selfPlaySettings.getMemoryFolder();
                std::filesystem::path oldFolder    = std::filesystem::path(memoryFolder).append("old");
                assert(memoryFolder.string() != oldFolder.string());
                if (!std::filesystem::exists(oldFolder))
                {
                    std::filesystem::create_directory(oldFolder);
                }
                for (auto& p: std::filesystem::directory_iterator(memoryFolder))
                {
                    if (p.path().extension() == ".bin")
                    {
                        // move to old folder
                        std::filesystem::path newPath = oldFolder / p.path().filename().string();
                        std::filesystem::rename(p.path(), newPath);
                    }
                }

                // TODO: evaluate with older model
                // LINFO << "Evaluating against old model...";
                // TODO: keep model with best performance

                // move old model to old/ folder
                LINFO << "Moving old model...";
                std::filesystem::path modelPath = std::filesystem::path(selfPlaySettings.getModelPath());
                if (modelPath.string().starts_with("./"))
                {
                    modelPath = modelPath.string().substr(2);
                }
                std::filesystem::path modelFolder  = modelPath.parent_path();
                std::filesystem::path oldModelPath = std::filesystem::path(modelFolder).append("old").append(modelPath.filename().string());
                if (!std::filesystem::exists(oldModelPath.parent_path()))
                {
                    std::filesystem::create_directory(oldModelPath.parent_path());
                }
                LINFO << "Moving " << modelPath << " to " << oldModelPath;
                std::filesystem::rename(modelPath, oldModelPath);

                LINFO << "Setting new model path to " << trainedModelName;
                trainerSettings.setModelPath(trainedModelName);
                selfPlaySettings.setModelPath(trainedModelName);
                // save new model
                LINFO << "Saving new model...";
            }
        }
        else
        {
            // run games infinitely
            SelfPlayTally tally;
            while (g_Running)
            {
                runGame(&selfPlaySettings, &tally);
            }
        }
    }

    return 0;
}