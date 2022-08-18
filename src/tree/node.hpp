#pragma once

#include "../connect4/environment.hpp"

class Node {
  public:
	Node(Node* parent, Environment* env, int move, float prior);
	Node(Environment* env);
	Node(const Node& node);
	~Node();

	Node *getParent();
	void setParent(Node *parent);

	std::vector<Node *> getChildren();
	void addChild(Node *child);

	Environment* getEnvironment();

	void incrementVisit();
	int getVisits();

	float getValue();
	void setValue(float value);

	float getPrior();
	void setPrior(float prior);

	float getQ();
	float getU();

	int getMove();

  private:
	Node *m_Parent = nullptr;
	std::vector<Node *> m_Children = {};
	Environment* m_Environment = nullptr;
	int m_Move = -1;
	float m_Prior = 0.0f;
	float m_Value = 0.0f;
	int m_Visits = 0;
};