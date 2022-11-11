#include "test.hpp"

namespace Test
{

void testHorizontalWin()
{
    LOG(INFO) << "Testing horizontal win";
    Environment env(6, 7);
    env.makeMove(0);
    env.makeMove(0);
    env.makeMove(1);
    env.makeMove(1);
    env.makeMove(2);
    env.makeMove(2);
    env.makeMove(3);
    env.print();

    assert(env.currentPlayerHasConnected4());
}

void testVerticalWin()
{
    LOG(INFO) << "Testing vertical win";
    Environment env(6, 7);
    env.makeMove(0);
    env.makeMove(1);
    env.makeMove(0);
    env.makeMove(1);
    env.makeMove(0);
    env.makeMove(1);
    env.makeMove(0);
    env.print();
    assert(env.currentPlayerHasConnected4());
}

void testDiagonalWin()
{
    LOG(INFO) << "Testing diagonal win (from bottom left to top right)";
    Environment env(6, 7);
    env.makeMove(0);
    env.makeMove(1);
    env.makeMove(1);
    env.makeMove(2);
    env.makeMove(2);
    env.makeMove(3);
    env.makeMove(2);
    env.makeMove(3);
    env.makeMove(3);
    env.makeMove(1);
    env.makeMove(3);
    env.print();
    assert(env.currentPlayerHasConnected4());

    LOG(INFO) << "Testing diagonal win (from top left to bottom right)";
    env.newEnvironment(6, 7);
    env.makeMove(0);
    env.makeMove(0);
    env.makeMove(0);
    env.makeMove(1);
    env.makeMove(0);
    env.makeMove(1);
    env.makeMove(3);
    env.makeMove(2);
    env.makeMove(2);
    env.makeMove(2);
    env.makeMove(1);
    assert(env.currentPlayerHasConnected4());
}

void testEasyPuzzle()
{
    SelfPlaySettings* settings = new SelfPlaySettings();
    settings->addAgent("Yellow", "models/model.pt", ePlayer::YELLOW);
    settings->addAgent("Red", "models/model.pt", ePlayer::RED);

    settings->setSimulations(1000);
    settings->setStochastic(false);
    settings->setShowMoves(true);
    settings->setSaveMemory(false);
    Game game = Game(settings);

    // for debugging purposes: try pre-configured board
    std::vector<uint8_t> data = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0,
    };
    auto          options = torch::TensorOptions().dtype(torch::kInt8);
    torch::Tensor board   = torch::from_blob(data.data(), {settings->getRows(), settings->getCols()}, options);
    game.getEnvironment()->setBoard(board);
    // game.getEnvironment()->setCurrentPlayer(ePlayer::RED);

    assert(game.playGame() == ePlayer::YELLOW);
}

void testStochasticDistribution()
{
    std::vector<int>                choices  = {1000, 1, 2, 2, 2, 2, 1};
    std::vector<int>                counters = std::vector<int>(choices.size(), 0);
    std::discrete_distribution<int> distribution(choices.begin(), choices.end());

    for (int i = 0; i < 100000; i++)
    {
        counters[distribution(g_Generator)]++;
    }

    for (int i = 0; i < (int)counters.size(); i++)
    {
        LOG(INFO) << "Choice " << i << ": " << counters[i];
    }
}
} // namespace Test
