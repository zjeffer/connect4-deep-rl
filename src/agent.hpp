#pragma once

#include <string>

#include "connect4/environment.hpp"
#include "mcts.hpp"
#include "neuralNetwork.hpp"
#include "tree/node.hpp"
#include "common.hpp"

/**
 * @brief An agent is used to represent a player. It holds the player's neural network.
 *
 */
class Agent
{
  public:
    /**
     * @brief Construct a new Agent
     *
     * @param name: Name to give this agent. Can be any string.
     * @param model_path: Path to the neural network
     * @param selfPlaySettings: other settings for selfplay
     */
    Agent(std::string name, std::string model_path, SelfPlaySettings* selfPlaySettings);

    /**
     * @brief Construct a new Agent with an already created neural network.
     *
     * @param name Name to give this agent. Can be any string.
     * @param nn A shared pointer to an alread loaded neural network.
     * @param selfPlaySettings: other settings for selfplay
     */
    Agent(std::string name, std::shared_ptr<NeuralNetwork> nn, SelfPlaySettings* selfPlaySettings);

    /**
     * @brief Destroy the Agent object
     *
     */
    ~Agent();

    /**
     * @brief Get the MCTS tree
     *
     * @return MCTS*
     */
    [[nodiscard]] MCTS* getMCTS() const;

    /**
     * @brief Get the Agent's name
     *
     * @return std::string
     */
    [[nodiscard]] std::string const& getName() const;
    /**
     * @brief Set the Agent's name
     *
     * @param name
     */
    void setName(std::string name);

  private:
    std::string                    m_Name;
    std::shared_ptr<NeuralNetwork> m_NN;
    MCTS*                          m_MCTS;
};