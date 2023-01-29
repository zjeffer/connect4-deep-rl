#include "game.hpp"

#include "connect4/player.hpp"

Game::Game(std::shared_ptr<Settings> settings, std::pair<std::shared_ptr<Agent>, std::shared_ptr<Agent>> const & agents) 
    : m_Settings(settings)
    , m_Env(std::make_shared<Environment>(m_Settings->getRows(), m_Settings->getCols()))
{
    m_Agents.emplace_back(agents.first);
    m_Agents.emplace_back(agents.second);

    // create a random id
    std::string current_date = std::to_string(std::time(nullptr));
    m_GameID                 = "game-" + current_date + "-" + std::to_string(g_UniformIntDist(g_Generator));
}

Game::~Game() {}

std::shared_ptr<Environment> Game::getEnvironment() const
{
    return m_Env;
}

ePlayer Game::playGame()
{
    ePlayer winner = ePlayer::NONE;

    m_Env->print();
    while (m_Env->hasValidMoves() && g_Running)
    {
        if (playMove())
        {
            // break if no moves left or someone won
            break;
        }
    }

    m_Env->togglePlayer();
    winner = m_Env->getWinner();

    if (!g_Running)
    {
        LINFO << "Game stopped";
        exit(EXIT_SUCCESS);
    }

    if (m_Settings->saveMemory())
    {
        updateMemoryWithWinner(winner);
        if (!saveMemoryToFile())
        {
            LFATAL << "Could not save memory to file";
        }
    }

    return winner;
}

bool Game::playMove()
{
    // play move and return if game is over
    std::cout << std::endl;

    // get agent
    int                    currentAgent = m_Env->getCurrentPlayer() == ePlayer::YELLOW ? 0 : m_Env->getCurrentPlayer() == ePlayer::RED ? 1 : -1;
    std::shared_ptr<Agent> agent        = m_Agents.at(currentAgent);
    std::shared_ptr<MCTS>  mcts         = agent->getMCTS();

    if (m_PreviousMoves.first != -1 && m_PreviousMoves.second != -1)
    {
        Node * newRoot = mcts->getRoot()->getChildAfterMove(m_PreviousMoves.first);
        if (newRoot == nullptr)
        {
            LFATAL << "NewRoot after getting first child is null!";
        }
        newRoot = newRoot->getChildAfterMove(m_PreviousMoves.second);
        if (newRoot == nullptr)
        {
            LFATAL << "NewRoot after getting second child is null!";
        }
        mcts->setRoot(newRoot);
    }
    else
    {
        mcts->setRoot(std::make_unique<Node>(m_Env));
    }

    mcts->run_simulations(m_Settings->getSimulations());

    std::unique_ptr<Node> const & currentRoot = mcts->getRoot();
    // calculate average action-value of all actions in the root node
    float value = 0.0f;
    for (auto & node: currentRoot->getChildren())
    {
        float weight = (float)node->getVisits() / (float)currentRoot->getVisits();
        value += node->getQ() * weight;
    }
    LINFO << "Average action-value according to current player (" << agent->getName() << "): " << value;

    // get best move from mcts tree
    int bestMove = m_Settings->isStochastic() ? mcts->getBestMoveStochastic() : mcts->getBestMoveDeterministic();

    // print moves and their q + u values
    std::vector<float> moveProbs = std::vector<float>(m_Env->getCols(), 0.0f);
    for (auto & child: currentRoot->getChildren())
    {
        moveProbs[child->getMove()] = (float)child->getVisits() / (float)currentRoot->getVisits();
        if (m_Settings->showMoves())
        {
            LDEBUG << "Move: " << child->getMove() << " Q: " << child->getQ() << " U: " << child->getU() << ". Visits: " << child->getVisits();
        }
    }

    if (m_Settings->saveMemory())
    {
        // create memory element
        MemoryElement element;
        element.board         = utils::boardToVector(m_Env->getBoard());
        element.currentPlayer = static_cast<uint8_t>(m_Env->getCurrentPlayer());
        element.moveList      = moveProbs;
        element.winner        = 0;
        // save element to memory
        addElementToMemory(element);

        if ((int)element.board.size() != m_Env->getRows() * m_Env->getCols())
        {
            LFATAL << "Memory element has no board";
        }
    }

    LINFO << "Playing best move: " << bestMove;

    // make the best move
    m_Env->makeMove(bestMove);
    m_Env->print();

    m_PreviousMoves.first  = m_PreviousMoves.second;
    m_PreviousMoves.second = bestMove;

    std::cout << "\n==============================================\n" << std::endl;
    return !m_Env->hasValidMoves() || m_Env->currentPlayerHasConnected4();

}

void Game::updateMemoryWithWinner(ePlayer winner)
{
    // update memory with winner
    for (MemoryElement & element: m_Memory)
    {
        element.winner = (winner == ePlayer::YELLOW) ? 1 : (winner == ePlayer::RED) ? -1 : 0;
    }
}

bool Game::saveMemoryToFile()
{
    std::filesystem::path folder = m_Settings->getMemoryFolder();
    if (!std::filesystem::exists(folder))
    {
        std::filesystem::create_directories(folder);
    }
    std::filesystem::path file = folder / (m_GameID + ".bin");
    return utils::writeMemoryElementsToFile(m_Memory, file);
}

void Game::addElementToMemory(MemoryElement element)
{
    m_Memory.push_back(element);
}