#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "../common.hpp"
#include "../connect4/player.hpp"
#include "settings.hpp"
#include "types.hpp"

/**
 * @brief Keeps the settings for self-play
 *
 */
class SelfPlaySettings : public Settings
{
  public:
    /**
     * @brief Construct a new settings object
     *
     */
    SelfPlaySettings();
    /**
     * @brief Load settings based on a settings file.
     * TODO: implement this.
     * @param settings_path: path to the file
     */
    SelfPlaySettings(std::filesystem::path const& settings_path);

    /**
     * @brief Get the amount of MCTS simulations
     *
     * @return int
     */
    [[nodiscard]] int getSimulations() const;
    /**
     * @brief Change the amount of MCTS simulations
     *
     * @param simulations
     */
    void setSimulations(int simulations);

    /**
     * @brief Return true if stochastic selection is enabled when choosing a move from the tree's root Node.
     *
     */
    [[nodiscard]] bool isStochastic() const;
    /**
     * @brief Set whether selection should be stochastic or deterministic.
     *
     * @param stochastic: true == stochastic, false == deterministic
     */
    void setStochastic(bool stochastic);

    /**
     * @brief Create a new AgentData object, to later create the Agent in the Game class.
     *
     * @param name: name of the agent. Can be any string.
     * @param model_path: the path to the neural network
     * @param player: which player this agent corresponds to.
     */
    void addAgent(std::string name, std::string const& model_path, ePlayer player);
    /**
     * @brief Get the list of Agents
     *
     * @return const std::vector<AgentData>&
     */
    std::vector<AgentData> const& getAgents() const;

    /**
     * @brief For debugging: show the root Node's actions after running simulations.
     *
     * @return true if the program must display the list of moves
     */
    bool showMoves() const;
    /**
     * @brief Set whether to show the list of moves or not
     *
     * @param show_moves
     */
    void setShowMoves(bool show_moves);

    /**
     * @brief Return true if self-play should save positions to memory
     */
    bool saveMemory() const;
    /**
     * @brief Set whether to save positions to memory or not
     *
     * @param save_memory
     */
    void setSaveMemory(bool save_memory);

    /**
     * @brief Get the amount of games to run every pipeline
     *
     * @return int
     */
    int getPipelineGames() const;

    /**
     * @brief Set the amount of games to run every pipeline
     *
     * @param games
     */
    void setPipelineGames(int games);

  private:
    int                    m_Simulations         = 200;
    bool                   m_UseStochasticSearch = true;
    std::vector<AgentData> m_AgentSettings       = std::vector<AgentData>();
    bool                   m_ShowMoves           = false;
    bool                   m_SaveMemory          = true;
    int                    m_PipelineGames       = 50;
};