#pragma once

#include "../connect4/environment.hpp"

/**
 * @brief A Node represents a position in the MCTS tree
 *
 */
class Node
{
  public:
    /**
     * @brief Construct a new Node
     *
     * @param parent: The previous Node
     * @param env: The environment that describes this position
     * @param move: The action the previous Node made to get here
     * @param prior: The probability of the action
     */
    Node(std::shared_ptr<Node> parent, std::shared_ptr<Environment> env, int move, float prior);
    /**
     * @brief Construct a new root node: no parent/move/prior.
     *
     * @param env: The environment
     */
    Node(std::shared_ptr<Environment> env);
    /**
     * @brief Copy constructor to copy a Node
     *
     * @param node: The node to copy
     */
    Node(Node const& node);

    /**
     * @brief Destroy the Node object
     *
     */
    ~Node();

    /**
     * @brief Get the Node's parent Node
     *
     * @return Node*
     */
    std::shared_ptr<Node> getParent() const;
    /**
     * @brief Change the Node's parent parent
     *
     * @param parent
     */
    void setParent(std::shared_ptr<Node> parent);

    /**
     * @brief Get a vector of child nodes for this Node.
     * These represent the resulting positions of all possible moves from the current position.
     *
     * @return const std::vector<std::unique_ptr<Node>>&
     */
    std::vector<std::shared_ptr<Node>> const& getChildren() const;

    /**
     * @brief Get the child that resulted after making 
     * the given move from the current node
     *
     * @param move: the move which resulted in the child
     * @return std::shared_ptr<Node> const&: the child
     */
    std::shared_ptr<Node> getChildAfterMove(int move);

    /**
     * @brief Add a new child to this Node. Moves unique_ptr's ownership.
     *
     * @param child
     */
    void addChild(std::shared_ptr<Node> child);

    /**
     * @brief Get this Node's environment.
     *
     * @return std::shared_ptr<Environment>
     */
    std::shared_ptr<Environment> getEnvironment() const;

    /**
     * @brief Increment the visit count for this Node.
     *
     */
    void incrementVisit();
    /**
     * @brief Get the visit count for this Node.
     *
     * @return int
     */
    int getVisits() const;

    /**
     * @brief Get this Node's current value
     *
     * @return float
     */
    float getValue() const;
    /**
     * @brief Update this Node's value.
     *
     * @param value
     */
    void setValue(float value);

    /**
     * @brief Get the prior probability
     *
     * @return float
     */
    float getPrior() const;
    /**
     * @brief Set the prior probability
     *
     * @param prior
     */
    void setPrior(float prior);

    /**
     * @brief Calculate the average value per visit
     *
     * @return float
     */
    float getQ() const;

    /**
     * @brief Calculate the Upper Confidence Bound (UCB)
     *
     * @return float
     */
    float getU() const;

    /**
     * @brief Get the move the parent made to get to this position
     *
     * @return int
     */
    int getMove();

  private:
    std::shared_ptr<Node>              m_Parent      = nullptr;
    std::vector<std::shared_ptr<Node>> m_Children    = {};
    std::shared_ptr<Environment>       m_Environment = nullptr;
    int                                m_Move        = -1;
    float                              m_Prior       = 0.0f;
    float                              m_Value       = 0.0f;
    int                                m_Visits      = 0;
};