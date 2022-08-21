#pragma once

#include "../connect4/environment.hpp"

class Node {
  public:
	Node(Node* parent, std::shared_ptr<Environment> env, int move, float prior);
	Node(std::shared_ptr<Environment> env);
	Node(const Node& node);
	~Node();

	Node *getParent();
	void setParent(Node *parent);

	const std::vector<std::unique_ptr<Node>>& getChildren() const;
	void addChild(std::unique_ptr<Node> child);

	std::shared_ptr<Environment> getEnvironment() const;

	void incrementVisit();
	int getVisits() const;

	float getValue() const;
	void setValue(float value);

	float getPrior() const;
	void setPrior(float prior);

	float getQ() const;
	float getU() const;

	int getMove();

  private:
	Node *m_Parent = nullptr;
	std::vector<std::unique_ptr<Node>> m_Children = {};
	std::shared_ptr<Environment> m_Environment = nullptr;
	int m_Move = -1;
	float m_Prior = 0.0f;
	float m_Value = 0.0f;
	int m_Visits = 0;
};