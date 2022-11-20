#include "node.hpp"

Node::Node(Node * parent, std::shared_ptr<Environment> env, int move, float prior)
  : m_Parent(parent)
  , m_Environment(env)
  , m_Move(move)
  , m_Prior(prior)
{
}

Node::Node(std::shared_ptr<Environment> env)
  : m_Environment(env)
{
    // root node, no parent, move or prior
}

Node::~Node() {}

std::vector<std::unique_ptr<Node>> const & Node::getChildren() const
{
    return m_Children;
}

Node * Node::getChildAfterMove(int move)
{
    for (auto const & node: m_Children)
    {
        if (node->getMove() == move)
        {
            return node.get();
        }
    }
    return nullptr;
}

void Node::addChild(std::unique_ptr<Node> child)
{
    m_Children.emplace_back(std::move(child));
}

Node* Node::removeChild(std::unique_ptr<Node> const & child)
{
    Node * releasedChild = nullptr;
    auto   found         = std::find_if(m_Children.begin(), m_Children.end(), [&](std::unique_ptr<Node> const & node) { return node == child; });
    if (found != m_Children.end())
    {
        releasedChild = (*found).release();
        m_Children.erase(found);
    }
    else
    {
        throw std::runtime_error("Could not remove child: child not in list of children");
    }
    return releasedChild;
}

Node * Node::getParent() const
{
    return m_Parent;
}

void Node::setParent(Node * parent)
{
    m_Parent = parent;
}

std::shared_ptr<Environment> const & Node::getEnvironment() const
{
    return m_Environment;
}

void Node::incrementVisit()
{
    m_Visits++;
}

int Node::getVisits() const
{
    return m_Visits;
}

float Node::getValue() const
{
    return m_Value;
}

void Node::setValue(float value)
{
    m_Value = value;
}

float Node::getPrior() const
{
    return m_Prior;
}

void Node::setPrior(float prior)
{
    m_Prior = prior;
}

float Node::getQ() const
{
    return m_Value / ((float)m_Visits + 1e-3);
}

float Node::getU() const
{
    if (m_Parent == nullptr)
    {
        LFATAL << "Parent is null. This shouldn't happen unless you call this function on the root node.";
    }
    // uses the PUCT formula based on AlphaZero's paper and pseudocode
    float exp_rate = log((m_Parent->getVisits() + 19652.0f + 1.0f) / 19652.0f) + 1.25f;
    exp_rate *= sqrt((float)m_Parent->getVisits()) / ((float)m_Visits + 1.0f);
    return exp_rate * this->m_Prior;
}

int Node::getMove()
{
    return m_Move;
}
