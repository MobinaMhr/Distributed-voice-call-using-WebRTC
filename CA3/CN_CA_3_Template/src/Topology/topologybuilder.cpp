#include "TopologyBuilder.h"

/// The node number is ambiguous

TopologyBuilder::TopologyBuilder(QObject *parent) :
    QObject(parent),
    m_macAddressGenerator(new MacAddressGenerator()) {}

TopologyBuilder::~TopologyBuilder() {
    delete m_macAddressGenerator;
    unbindAllPorts();
}

void TopologyBuilder::unbindAllPorts() {
    for (auto port : m_ports) {
        m_portBindingManager.unbind(port);
    }
}

QSharedPointer<Router> TopologyBuilder::createRouter(int id) {
    return QSharedPointer<Router>::create(
      id,
      m_macAddressGenerator->generateMacAddress(),
      UT::IPVersion::IPv4,
      nullptr
    );
}

void TopologyBuilder::createMeshTopology(int rows, int columns) {
    m_nodes.resize(rows * columns);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            int id = i * columns + j;
            auto node = createRouter(id++);
            m_nodes[id] = node;

            if (j > 0) bindPorts(m_nodes[id], m_nodes[id - 1]);
            if (i > 0) bindPorts(m_nodes[id], m_nodes[id - columns]);
        }
    }
}

void TopologyBuilder::createTorusTopology(int rows, int columns) {
    createMeshTopology(rows, columns);

    for (int i = 0; i < rows; ++i) {
        bindPorts(m_nodes[i * columns], m_nodes[i * columns + columns - 1]);
    }

    for (int j = 0; j < columns; ++j) {
        bindPorts(m_nodes[j], m_nodes[(rows - 1) * columns + j]);
    }
}

void TopologyBuilder::createStarTopology(int numNodes) {
    auto centerNode = createRouter(1);
    m_nodes.push_back(centerNode);

    for (int i = 1; i <= numNodes; ++i) {
        auto node = createRouter(i++);
        m_nodes.push_back(node);
        bindPorts(centerNode, node);
    }
}

void TopologyBuilder::createRingStarTopology(int numNodes) {
    if (numNodes < 3) return;

    auto centerNode = createRouter(1);
    m_nodes.push_back(centerNode);

    for (int i = 1; i < numNodes; ++i) {
        auto node = createRouter(i++);
        m_nodes.push_back(node);
        bindPorts(centerNode, node);

        if (i % 2) bindPorts(m_nodes[i], m_nodes[i - 1]);
    }
    bindPorts(m_nodes[1], m_nodes[numNodes]);
}

void TopologyBuilder::bindPorts(QSharedPointer<Router> node1, QSharedPointer<Router> node2) {
    if (!node1 || !node2) {
        qDebug() << "Invalid nodes passed to bindPorts.";
        return;
    }

    PortPtr_t port1 = m_portBindingManager.createPort();
    PortPtr_t port2 = m_portBindingManager.createPort();

    m_portBindingManager.bind(port1, port2);

    m_ports.push_back(port1);
    m_ports.push_back(port2);
}

void TopologyBuilder::unbindPorts(QSharedPointer<Router> node1, QSharedPointer<Router> node2) {
    if (!node1 || !node2) {
        qDebug() << "Invalid nodes passed to unbindPorts.";
        return;
    }

    PortPtr_t port1 = m_portBindingManager.createPort();
    PortPtr_t port2 = m_portBindingManager.createPort();

    m_portBindingManager.unbind(port1, port2);

    m_ports.removeAll(port1);
    m_ports.removeAll(port2);
}

QVector<QSharedPointer<Router>> TopologyBuilder::nodes() {
    return m_nodes;
}
