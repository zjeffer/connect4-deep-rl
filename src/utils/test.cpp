#include "test.hpp"

#include <c10/util/ArrayRef.h>

#include <cstdint>

#include "types.hpp"
#include "utils.hpp"
#include <ATen/ops/from_blob.h>

namespace Test
{

void testHorizontalWin()
{
    LINFO << "Testing horizontal win";
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
    LINFO << "Testing vertical win";
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
    LINFO << "Testing diagonal win (from bottom left to top right)";
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

    LINFO << "Testing diagonal win (from top left to bottom right)";
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
    std::shared_ptr<SelfPlaySettings> settings = std::make_shared<SelfPlaySettings>();
    settings->addAgent("Yellow", "models/model.pt", ePlayer::YELLOW);
    settings->addAgent("Red", "models/model.pt", ePlayer::RED);

    settings->setSimulations(1000);
    settings->setStochastic(false);
    settings->setShowMoves(true);
    settings->setSaveMemory(false);
    Game game = Game(settings);

    // for debugging purposes: try pre-configured board
    std::vector<uint8_t> data = {
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 2, 2, 2, 0, 0, 0, //
      0, 1, 1, 1, 0, 0, 0  //
    };
    auto          options = torch::TensorOptions().dtype(torch::kInt8);
    torch::Tensor board   = torch::from_blob(data.data(), {settings->getRows(), settings->getCols()}, options);
    game.getEnvironment()->setBoard(board);
    // game.getEnvironment()->setCurrentPlayer(ePlayer::RED);

    assert(game.playGame() == ePlayer::YELLOW);
}

void testStochasticDistribution()
{
    LDEBUG << "Testing stochastic distribution...";
    std::vector<int>                choices  = {28, 9, 172, 22, 20, 10, 19};
    std::vector<int>                counters = std::vector<int>(choices.size(), 0);
    std::discrete_distribution<int> distribution(choices.begin(), choices.end());

    int amount = 10000000;
    for (int i = 0; i < amount; i++)
    {
        counters[distribution(g_Generator)]++;
    }

    for (int i = 0; i < (int)counters.size(); i++)
    {
        LINFO << "Choice " << i << ": " << counters[i] << "   \t" << 100 * (float)counters[i] / (float)amount << "%";
    }
}

void testReadAndWriteMemoryElement()
{
    if (std::remove("test/test.bin") == 0)
    {
        LINFO << "test.bin file deleted.";
    }
    else
    {
        LINFO << "test.bin file not deleted.";
    }

    std::vector<uint8_t> data = {
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 2, 2, 0, 0, 0, 0, //
      0, 1, 1, 1, 0, 0, 0, //
    };

    MemoryElement element;
    element.board         = data;
    element.currentPlayer = static_cast<uint8_t>(ePlayer::RED);
    element.moveList      = {0.1f, 0.1f, 0.1f, 0.1f, 0.4f, 0.1f, 0.1f};
    element.winner        = static_cast<uint8_t>(ePlayer::YELLOW);

    std::vector<uint8_t> data2 = {
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 0, 0, 0, 0, 0, 0, //
      0, 2, 2, 0, 0, 0, 0, //
      0, 1, 1, 1, 2, 0, 0, //
    };

    MemoryElement element2;
    element2.board         = data2;
    element2.currentPlayer = static_cast<uint8_t>(ePlayer::YELLOW);
    element2.moveList      = {0.1f, 0.1f, 0.1f, 0.4f, 0.1f, 0.1f, 0.1f};
    element2.winner        = static_cast<uint8_t>(ePlayer::YELLOW);

    std::vector<MemoryElement> elements = {element, element2};
    utils::writeMemoryElementsToFile(elements, "test/test.bin");

    std::vector<MemoryElement> readElements;
    utils::readMemoryElementsFromFile(readElements, "test/test.bin");

    int i = 0;
    std::cout << std::endl;
    for (auto const& e: readElements)
    {
        std::cout << "Element " << i << ": ";
        std::cout << e.board << std::endl;
        std::cout << e.currentPlayer << std::endl;
        std::cout << e.moveList << std::endl;
        std::cout << e.winner << std::endl;
        i++;
    }
    std::cout << "Test complete" << std::endl;
}

} // namespace Test
