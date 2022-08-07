#include "node.hpp"

Node::Node(Node *parent, Environment* env, int move, float prior) {
	m_Parent = parent;
	m_Environment = env;
	m_Move = move;
	m_Prior = prior;
}

Node::Node(Environment* env) {
	m_Environment = env;
}

Node::~Node() {}


std::vector<Node *> Node::getChildren() {
	return m_Children;
}

void Node::addChild(Node *child) {
	m_Children.push_back(child);
}

Node* Node::getParent() {
	return m_Parent;
}

Environment* Node::getEnvironment() {
	return m_Environment;
}

void Node::incrementVisit() {
	m_Visits++;
}

int Node::getVisits() {
	return m_Visits;
}

float Node::getValue() {
	return m_Value;
}

void Node::setValue(float value) {
	m_Value = value;
}

float Node::getPrior() {
	return m_Prior;
}

void Node::setPrior(float prior) {
	m_Prior = prior;
}

float Node::getQ() {
	return m_Value / (m_Visits + 1e-6);
}

float Node::getU() {
	if (m_Parent == nullptr){
		LOG(FATAL) << "Parent is null";
		exit(EXIT_FAILURE);
	}
	float exp_rate = log((m_Parent->getVisits() + 19652.0f + 1.0f) / 19652.0f) + 1.25f;
	exp_rate *= sqrt(m_Parent->getVisits() + 1.0f) / (m_Visits + 1.0f);
	return exp_rate * this->m_Prior;
}


int Node::getMove() {
	return m_Move;
}
