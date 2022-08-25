#pragma once

#include "neuralNetwork.hpp"
#include "utils/selfPlaySettings.hpp"
#include "tree/node.hpp"
#include "utils/tqdm.h"
#include "common.hpp"

/**
 * @brief The MCTS class is responsible for running the MCTS simulations,
 * and keeping the MCTS tree.
 *
 */
class MCTS {
  public:
	/**
	 * @brief Construct a new MCTS tree
	 *
	 * @param selfPlaySettings
	 * @param root: the Node where the tree starts from
	 * @param nn: the neural network to use in the expand() method
	 */
	MCTS(SelfPlaySettings* selfPlaySettings, Node* root, const std::shared_ptr<NeuralNetwork>& nn);
	~MCTS();

	/**
	 * @brief Continuously run the 4 steps of the MCTS algorithm.
	 * The amount of simulations is taken from the given selfPlaySettings
	 */
	void run_simulations();

	/**
	 * @brief The first step of the MCTS algorithm: keep selecting actions until a
	 * position (Node) has been reached that has not yet been visited (expanded)
	 *
	 * @param root: the root node of the tree, where the selection will start.
	 * @return Node*: the leaf node that has not yet been expanded
	 */
	Node* select(Node* root);

	/**
	 * @brief The 2nd and 3rd steps of the MCTS algorithm: Expand the given leaf node
	 * and evaluate the value of the leaf node.
	 *
	 * @param leaf: the leaf node found by the select() method
	 * @return float: the value of the leaf node (from the NN)
	 */
	float expand(Node* leaf);

	/**
	 * @brief The 4th and final step of the MCTS algorithm: Backpropagate the value
	 * through the tree, up to the root node.
	 *
	 * @param leaf: the bottom node to start from
	 * @param result: the value to backpropagate
	 */
	void backpropagate(Node* leaf, float result);

	/**
	 * @brief Get the root node of the tree
	 *
	 * @return Node*
	 */
	Node* getRoot() const;
	/**
	 * @brief Set a new root Node
	 *
	 * @param root
	 */
	void setRoot(Node* root);

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
	static int getTreeDepth(Node* root);

  private:
	SelfPlaySettings* m_Settings = nullptr;
	Node* m_Root = nullptr;
	std::shared_ptr<NeuralNetwork> m_NN = nullptr;
	torch::Device m_Device = torch::kCPU;
};