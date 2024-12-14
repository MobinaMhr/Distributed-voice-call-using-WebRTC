#include "Node.h"

Node::Node(int id, const MacAddress &macAddress, int portCount, UT::IPVersion ipVersion, QThread* parent)
    : QThread(parent),
    m_id(id),
    m_macAddress(macAddress),
    m_name("Node_" + QString::number(id)),
    m_ipVersion(ipVersion) {
    m_state = UT::NodeState::Alive;
}

Node::~Node() {}

// Node* Node::instance(int id, const MacAddress &macAddress, int portCount, QThread* parent) {
//     if (!m_self) {
//         m_self = new Node(id, macAddress, parent);
//     }
//     return m_self;
// }

void Node::release() {
    if (m_self) {
        delete m_self;
        m_self = nullptr;
    }
}

void Node::run() {
    exec();
}

int Node::id() const {
    return m_id;
}

QString Node::name() const {
    return m_name;
}

QString Node::macAddress() const {
    return m_macAddress.toString();
}


UT::NodeState Node::state() {
    return m_state;
}
void Node::setState(UT::NodeState state) {
    m_state = state;
}

QString Node::ipv6Address() const
{
    return m_ipv6Address.toString();
}

QString Node::ipv4Address() const {
    return m_ipv4Address.toString();
}
