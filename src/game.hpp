#pragma once

#include "agent.hpp"
#include "common.hpp"
#include "connect4/environment.hpp"
#include "utils/selfPlaySettings.hpp"
#include "utils/types.hpp"

/**
 * @brief The Game class is for creating an environment and agents
 * to self-play games of Connect 4.
 */
class Game
{
  private:
    std::shared_ptr<SelfPlaySettings> m_Settings      = nullptr;
    std::shared_ptr<Environment>      m_Env           = nullptr;
    std::string                       m_GameID        = "";
    std::pair<int, int>               m_PreviousMoves = std::make_pair<int, int>(-1, -1);

    std::vector<std::shared_ptr<Agent>> m_Agents = std::vector<std::shared_ptr<Agent>>();

    std::vector<MemoryElement> m_Memory;

  public:
    /**
     * @brief Construct a new Game based on the givenselfplay settings
     *
     * @param selfPlaySettings
     */
    Game(std::shared_ptr<SelfPlaySettings> selfPlaySettings);

    /**
     * @brief Destroy the Game object
     *
     */
    ~Game();

    /**
     * @brief Plays a single game of Connect 4.
     *
     * @return ePlayer: The winner of the game. ePlayer::None if draw.
     */
    ePlayer playGame();

    /**
     * @brief Plays a single move in the current environment.
     *
     * @return true if game is finished.
     */
    bool playMove();

    /**
     * @brief Add a single element to the current memory.
     * Does not include the winner, which needs to be added at the end of the game.
     *
     * @param element
     */
    void addElementToMemory(MemoryElement element);

    /**
     * @brief For every element in memory, set the winner.
     *
     * @param winner
     */
    void updateMemoryWithWinner(ePlayer winner);

    /**
     * @brief Save the current memory to a file in the chosen memory folder.
     *
     * @return true if successful
     */
    bool saveMemoryToFile();

    /**
     * @brief Get a pointer to the current environment
     *
     * @return std::shared_ptr<Environment>
     */
    std::shared_ptr<Environment> getEnvironment() const;
};