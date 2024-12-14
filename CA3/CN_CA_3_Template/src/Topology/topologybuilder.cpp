#include "TopologyBuilder.h"

/// The node number is ambiguous
///
/// /// pointer tor handle she

TopologyBuilder::TopologyBuilder(QObject *parent) :
    QObject(parent),
    m_macAddressGenerator(new MacAddressGenerator(this)) {}

TopologyBuilder::~TopologyBuilder() {
    delete m_macAddressGenerator;
    unbindAllPorts();
}

void TopologyBuilder::unbindAllPorts() {
    for (auto port : m_ports) {
        m_portBindingManager.unbind(port);
    }
}

QSharedPointer<Router> TopologyBuilder::createRouter(int id, int portCount) {
    return QSharedPointer<Router>::create(
      id,
      m_macAddressGenerator->generateMacAddress(),
      portCount,
      UT::IPVersion::IPv4,
      nullptr
    );
}

void TopologyBuilder::initializeRouters(int count, int offset) {
    m_offset = offset;
    m_routerCount = count;
    m_routers.resize(m_routerCount);

    for (int id = 0; id < m_routerCount; ++id) {
        auto router = createRouter(id + 1 + m_offset, 4); /// TODO - Hard code
        m_routers[id] = router;
    }
}

void TopologyBuilder::moveToMeshTopology(int rows, int columns) {
    unbindAllPorts();

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            int id = i * columns + j;
            if (j > 0) bindPorts(m_routers[id], m_routers[id - 1]);
            if (i > 0) bindPorts(m_routers[id], m_routers[id - columns]);
        }
    }
}

void TopologyBuilder::moveToTorusTopology(int rows, int columns) {
    unbindAllPorts();
    moveToMeshTopology(rows, columns);

    for (int i = 0; i < rows; ++i) {
        bindPorts(m_routers[i * columns], m_routers[i * columns + columns - 1]);
    }

    for (int j = 0; j < columns; ++j) {
        bindPorts(m_routers[j], m_routers[(rows - 1) * columns + j]);
    }
}

void TopologyBuilder::moveToStarTopology() {
    unbindAllPorts();

    auto centerNode = m_routers[0];
    for (int i = 1; i <= m_routerCount; ++i) {
        auto node = m_routers[i];
        bindPorts(centerNode, node);
    }
}

void TopologyBuilder::moveToRingStarTopology() {
    if (m_routerCount < 3) return;

    unbindAllPorts();
    auto centerNode = m_routers.back();

    int i = 0;
    for (i = 0; i < m_routerCount - 1; ++i) {
        auto node = m_routers[i];
        if (i % 2 == 0) bindPorts(centerNode, node);
        if (i > 0)      bindPorts(m_routers[i], m_routers[i - 1]);
    }
    bindPorts(m_routers[i-1], m_routers[0]);
}

void TopologyBuilder::bindPorts(QSharedPointer<Router> router1, QSharedPointer<Router> router2) {
    if (!router1 || !router2) {
        qDebug() << "Invalid routers passed to bindPorts.";
        return;
    }

    PortPtr_t port1 = m_portBindingManager.createPort(); // DO NOT CREATE Ports HEARE AND GET THEM FROM ROUTERS
    PortPtr_t port2 = m_portBindingManager.createPort();

    m_portBindingManager.bind(port1, port2);

    m_ports.push_back(port1);
    m_ports.push_back(port2);
}

void TopologyBuilder::unbindPorts(QSharedPointer<Router> router1, QSharedPointer<Router> router2) {
    if (!router1 || !router2) {
        qDebug() << "Invalid routers passed to unbindPorts.";
        return;
    }

    PortPtr_t port1 = router1->getIdlePort();
    // if (port1 == nullptr)
    PortPtr_t port2 = router2->getIdlePort();
    // if (port2 == nullptr)

    m_portBindingManager.unbind(port1, port2);

    m_ports.removeAll(port1);
    m_ports.removeAll(port2);
}

QVector<QSharedPointer<Router>> TopologyBuilder::routers() {
    return m_routers;
}
