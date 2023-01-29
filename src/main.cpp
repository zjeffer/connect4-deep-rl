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
#include "utils/inputParser.hpp"
#include "utils/settings.hpp"
#include "utils/test.hpp"
#include "utils/types.hpp"

void signal_handling(int signal)
{
    std::cerr << "Signal " << signal << " received. Quitting..." << std::endl;
    g_Running = false;
}

[[noreturn]] void printUsage(std::string const & filename)
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

void parseOptions(InputParser const & inputParser, std::shared_ptr<Settings> settings)
{
    // set AI model
    if (inputParser.cmdOptionExists("--model"))
    {
        std::string modelPath = inputParser.getCmdOption("--model");
        settings->setModelPath(modelPath);
    }

    // FOR DEBUGGING PURPOSES: show Q+U+visits for every possible action
    settings->setShowMoves(true);

    // set #simulations per move
    if (inputParser.cmdOptionExists("--sims"))
    {
        try
        {
            int sims = std::stoi(inputParser.getCmdOption("--sims"));
            settings->setSimulations(sims);
            if (sims < 1)
            {
                throw std::invalid_argument(
                  "Amount must be greater or equal "
                  "to 1");
            }
        }
        catch (std::invalid_argument & e)
        {
            std::cerr << "Invalid argument for --sims: " << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    // set memory folder
    try
    {
        if (inputParser.cmdOptionExists("--memory-folder"))
        {
            settings->setMemoryFolder(inputParser.getCmdOption("--memory-folder"));
        }
    }
    catch (std::exception const & e)
    {
        LFATAL << "Invalid memory folder: " << e.what();
    }

    // set amount of pipeline games
    if (inputParser.cmdOptionExists("--pipeline"))
    {
        std::string games = inputParser.getCmdOption("--pipeline");
        if (games.length() > 0 && !games.starts_with(("-")))
        {
            try
            {
                settings->setPipelineGames(std::stoi(games));
                LINFO << "Pipeline: " << settings->getPipelineGames() << " games.";
            }
            catch (std::invalid_argument const & e)
            {
                LWARN << "Invalid amount of games: " << e.what();
            }
        }
    }

    try
    {
        if (inputParser.cmdOptionExists("--bs"))
        {
            settings->setBatchSize(std::stoi(inputParser.getCmdOption("--bs")));
        }
    }
    catch (std::invalid_argument const & e)
    {
        LFATAL << "Invalid batch size: " << e.what();
    }

    try
    {
        if (inputParser.cmdOptionExists("--lr"))
        {
            settings->setLearningRate(std::stod(inputParser.getCmdOption("--lr")));
        }
    }
    catch (std::invalid_argument const & e)
    {
        LFATAL << "Invalid learning rate: " << e.what();
    }

    try
    {
        if (inputParser.cmdOptionExists("--epochs"))
        {
            settings->setEpochs(std::stoi(inputParser.getCmdOption("--epochs")));
        }
    }
    catch (std::exception const & e)
    {
        LFATAL << "Invalid amount of epochs: " << e.what();
    }
}

/**
 @brief Return true if newer model is better
 */
bool evaluateModel(std::filesystem::path oldModel, std::filesystem::path newModel)
{
    int score                  = 0;
    int amountOfGamesPerPlayer = 10;

    std::shared_ptr<Settings> oldModelSettings = std::make_shared<Settings>();
    oldModelSettings->setSaveMemory(false);
    oldModelSettings->setSimulations(400);
    oldModelSettings->setStochastic(false);

    std::shared_ptr<Settings> newModelSettings = std::make_shared<Settings>();
    newModelSettings->setSaveMemory(false);
    newModelSettings->setSimulations(400);
    newModelSettings->setStochastic(false);

    // oldmodel starts as yellow, newmodel as red
    std::pair<std::shared_ptr<Agent>, std::shared_ptr<Agent>> agents;

    std::shared_ptr<Agent> oldAgent = std::make_shared<Agent>("OldAgent", oldModel, oldModelSettings);
    std::shared_ptr<Agent> newAgent = std::make_shared<Agent>("NewAgent", newModel, newModelSettings);
    agents.first                    = oldAgent;
    agents.second                   = newAgent;

    assert(agents.first->getName() == "OldAgent");
    assert(agents.second->getName() == "NewAgent");
    LINFO << "Evaluating " << oldModel << "as yellow vs " << newModel << " as red";
    for (int i = 0; i < amountOfGamesPerPlayer; i++)
    {
        Game    match  = Game(oldModelSettings, agents);
        ePlayer result = match.playGame();
        if (result == ePlayer::YELLOW)
        {
            // oldmodel wins
            score--;
        }
        else if (result == ePlayer::RED)
        {
            score++;
        }
    }

    LINFO << "Evaluating " << newModel << "as yellow vs " << oldModel << " as red";
    // swap agents, now oldmodel starts as red, newmodel as yellow
    std::swap(agents.first, agents.second);
    assert(agents.first->getName() == "NewAgent");
    assert(agents.second->getName() == "OldAgent");
    for (int i = 0; i < amountOfGamesPerPlayer; i++)
    {
        Game    match  = Game(oldModelSettings, agents);
        ePlayer result = match.playGame();
        if (result == ePlayer::YELLOW)
        {
            // newmodel wins
            score++;
        }
        else if (result == ePlayer::RED)
        {
            score--;
        }
    }

    return score > 0;
}

void runGame(std::shared_ptr<Settings> settings, std::shared_ptr<Agent> yellow, std::shared_ptr<Agent> red, SelfPlayTally & tally)
{
    Game    game   = Game(settings, std::pair(yellow, red));
    ePlayer winner = game.playGame();
    if (winner == ePlayer::NONE)
    {
        tally.draws++;
    }
    else if (winner == ePlayer::YELLOW)
    {
        tally.yellow++;
    }
    else if (winner == ePlayer::RED)
    {
        tally.red++;
    }
    LINFO << "Winner: " << static_cast<int>(winner) << "\n";
    LINFO << "========= Tally:";
    LINFO << "===== Yellow: " << tally.yellow;
    LINFO << "===== Red: " << tally.red;
    LINFO << "===== Draws: " << tally.draws;
    LINFO << "===============  \n";
    LINFO << "\n\n\n";
}

void runPipeline(std::shared_ptr<Settings> settings, std::shared_ptr<Agent> yellow, std::shared_ptr<Agent> red, SelfPlayTally & tally)
{
    LINFO << "Running selfplay...";
    for (int gameCount = 1; gameCount <= settings->getPipelineGames(); gameCount++)
    {
        LINFO << "\n\n\tStarting game " << gameCount << "...\n";
        runGame(settings, yellow, red, tally);
    }

    // train with these games
    LINFO << "Creating trainer...";
    Trainer trainer = Trainer(settings);
    LINFO << "Training new model...";
    std::filesystem::path trainedModelName = trainer.train();

    // move memory to old/ folder
    LINFO << "Moving old games...";
    std::filesystem::path memoryFolder    = settings->getMemoryFolder();
    std::filesystem::path oldMemoryFolder = memoryFolder / "old";
    if (!std::filesystem::exists(oldMemoryFolder))
    {
        std::filesystem::create_directories(oldMemoryFolder);
    }
    // move every memory file to old/ folder
    for (auto & p: std::filesystem::directory_iterator(memoryFolder))
    {
        if (p.path().extension() == ".bin")
        {
            // move to old folder
            std::filesystem::path newPath = oldMemoryFolder / p.path().filename().string();
            std::filesystem::rename(p.path(), newPath);
        }
    }

    LINFO << "Evaluating against old model...";
    if (evaluateModel(settings->getModelPath(), trainedModelName))
    {
        // new model is better: keep new model
        LINFO << "New model '" << trainedModelName << "' is better!";

        // move old model to old/ folder
        LDEBUG << "Moving old model...";
        std::filesystem::path modelPath    = settings->getModelPath();
        std::filesystem::path modelFolder  = modelPath.parent_path();
        std::filesystem::path oldModelPath = modelFolder / "old" / modelPath.filename();

        // create the old/ folder if it doesn't exist yet
        if (!std::filesystem::exists(oldModelPath.parent_path()))
        {
            std::filesystem::create_directories(oldModelPath.parent_path());
        }
        LDEBUG << "Moving " << modelPath << " to " << oldModelPath;
        std::filesystem::rename(modelPath, oldModelPath);

        LINFO << "Setting new model to " << trainedModelName;
        settings->setModelPath(trainedModelName);
    }
    else
    {
        LINFO << "New model is worse, reusing old model";
        // new model is worse, remove new model
        std::filesystem::remove(trainedModelName);
    }
}

int main(int argc, char * argv[])
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
        Test::runTests();
        return 0;
    }

    std::shared_ptr<Settings> settings = std::make_shared<Settings>();
    parseOptions(inputParser, settings);

    // TODO: load all settings from a json file or something
    if (inputParser.cmdOptionExists("--train"))
    {
        // parse settings
        // create trainer and train
        Trainer trainer = Trainer(settings);
        trainer.train();
        return 0;
    }
    else
    {
        // selfplay
        SelfPlayTally                  tally;
        std::shared_ptr<NeuralNetwork> model   = std::make_shared<NeuralNetwork>(settings);
        std::shared_ptr<Agent>         player1 = std::make_shared<Agent>("yellow", model, settings);
        std::shared_ptr<Agent>         player2 = std::make_shared<Agent>("red", model, settings);

        if (inputParser.cmdOptionExists("--pipeline"))
        {
            // run full pipeline with selfplay & training
            LINFO << "Running full pipeline with " << settings->getPipelineGames() << " games...";
            while (g_Running)
            {
                runPipeline(settings, player1, player2, tally);
            }
        }
        else
        {
            // run games infinitely
            while (g_Running)
            {
                runGame(settings, player1, player2, tally);
            }
        }
    }

    return 0;
}