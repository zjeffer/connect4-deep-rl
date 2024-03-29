#pragma once

#include "common.hpp"
#include "neuralNetwork.hpp"
#include "tree/node.hpp"
#include "utils/settings.hpp"
#include "utils/tqdm.h"

/**
 * @brief The MCTS class is responsible for running the MCTS simulations,
 * and keeping the MCTS tree.
 *
 */
class MCTS
{
  public:
    /**
     * @brief Construct a new MCTS tree
     *
     * @param settings
     * @param root: the Node where the tree starts from
     * @param nn: the neural network to use in the expand() method
     */
    MCTS(std::shared_ptr<Settings> settings, std::unique_ptr<Node> root, std::shared_ptr<NeuralNetwork> const & nn);
    ~MCTS();

    /**
     * @brief Continuously run the 4 steps of the MCTS algorithm.
     * The amount of simulations is taken from the given settings
     */
    void run_simulations(int simulations);

    /**
     * @brief The first step of the MCTS algorithm: keep selecting actions until a
     * position (Node) has been reached that has not yet been visited (expanded)
     *
     * @param root: the root node of the tree, where the selection will start.
     * @return Node*: the leaf node that has not yet been expanded
     */
    Node * select(Node* root);

    /**
     * @brief The 2nd and 3rd steps of the MCTS algorithm: Expand the given leaf node
     * and evaluate the value of the leaf node.
     *
     * @param leaf: the leaf node found by the select() method
     * @return float: the value of the leaf node (from the NN)
     */
    float expand(Node * node);

    /**
     * @brief The 4th and final step of the MCTS algorithm: Backpropagate the value
     * through the tree, up to the root node.
     *
     * @param leaf: the bottom node to start from
     * @param result: the value to backpropagate
     */
    void backpropagate(Node * leaf, float result);

    /**
     * @brief Get the root node of the tree
     *
     * @return Node*
     */
    std::unique_ptr<Node> const & getRoot() const;
    /**
     * @brief Set a new root Node
     *
     * @param root
     */
    void setRoot(Node * root);
    void setRoot(std::unique_ptr<Node> root);

    void addDirichletNoise(Node* root);

    /**
     * @brief After running simulations, get the root's best child node (deterministically).
     *
     * @return int: The move with highest visit count
     */
    int getBestMoveDeterministic() const;

    /**
     * @brief After running simulations, get the root's best child node (stochastically).
     *
     * @return int: The move that was selected with stochastic picking.
     */
    int getBestMoveStochastic() const;

    /**
     * @brief Get the depth of the tree, recursively.
     *
     * @param root: the node to start counting from.
     * @return int: the depth of the tree from the given node to the end
     */
    static int getTreeDepth(std::unique_ptr<Node> const & root);

  private:
    std::shared_ptr<Settings> m_Settings = nullptr;
    std::unique_ptr<Node>             m_Root     = nullptr;
    std::shared_ptr<NeuralNetwork>    m_NN       = nullptr;
    torch::Device                     m_Device   = torch::kCPU;
};