#include "node.hpp"
#include <memory>

Node::Node(Node* parent, std::shared_ptr<Environment> env, int move, float prior)
	: m_Parent(parent), m_Environment(env), m_Move(move), m_Prior(prior) {}

Node::Node(std::shared_ptr<Environment> env) : m_Environment(env) {
	// root node, no parent, move or prior
}

Node::~Node() {
	for (auto& node : m_Children) {
		delete node.release();
	}
}

Node::Node(const Node& node) {
	m_Parent = node.m_Parent;
	m_Environment = node.m_Environment;
	m_Move = node.m_Move;
	m_Prior = node.m_Prior;
	m_Value = node.m_Value;
	m_Visits = node.m_Visits;
}

const std::vector<std::unique_ptr<Node>>& Node::getChildren() const { return m_Children; }

void Node::addChild(std::unique_ptr<Node> child) { m_Children.push_back(std::move(child)); }

Node* Node::getParent() const { return m_Parent; }

std::shared_ptr<Environment> Node::getEnvironment() const { return m_Environment; }

void Node::incrementVisit() { m_Visits++; }

int Node::getVisits() const { return m_Visits; }

float Node::getValue() const { return m_Value; }

void Node::setValue(float value) { m_Value = value; }

float Node::getPrior() const { return m_Prior; }

void Node::setPrior(float prior) { m_Prior = prior; }

float Node::getQ() const { return m_Value / (float)(m_Visits + 1e-6); }

float Node::getU() const {
	if (m_Parent == nullptr) {
		// this should never happen unless you call this function on the root node
		LOG(FATAL) << "Parent is null";
		exit(EXIT_FAILURE);
	}
	// uses the PUCT formula based on AlphaZero's paper and pseudocode
	float exp_rate = log((m_Parent->getVisits() + 19652.0f + 1.0f) / 19652.0f) + 1.25f;
	exp_rate *= sqrt(m_Parent->getVisits() + 1.0f) / (m_Visits + 1.0f);
	return exp_rate * this->m_Prior;
}

int Node::getMove() { return m_Move; }
