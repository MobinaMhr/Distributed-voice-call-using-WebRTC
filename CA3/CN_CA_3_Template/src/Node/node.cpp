#include "Node.h"

Node::Node(int id, const MacAddress &macAddress, QThread* parent)
    : QThread(parent), m_id(id), m_macAddress(macAddress), m_name("Node_" + QString::number(id)) {}

Node::~Node() {}

// Node* Node::instance(int id, const MacAddress &macAddress, QThread *parent) {
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

void Node::processPacket(const QString &packetInfo) {
    qDebug() << "Node" << m_name << "processing packet:" << packetInfo;
}
