#include "TopologyBuilder.h"

/// The node number is ambiguous
///
/// /// pointer tor handle she

TopologyBuilder::TopologyBuilder(QObject *parent) :
    QObject(parent),
    m_macAddressGenerator(new MacAddressGenerator(this)) {}

TopologyBuilder::~TopologyBuilder() {
    delete m_macAddressGenerator;
    resetBindings(); ///Could be deleted
}

void TopologyBuilder::resetBindings() {
    for (auto &router : m_routers) {
        auto ports = router->getPorts();
        for (auto &port : ports) {
            m_portBindingManager.unbind(port);
        }
    }
}

QSharedPointer<Router> TopologyBuilder::createRouter(int id, int portCount, UT::IPVersion ipVersion) {
    return QSharedPointer<Router>::create(
      id,
      m_macAddressGenerator->generateMacAddress(),
      portCount,
      ipVersion,
      nullptr
    );
}

void TopologyBuilder::initializeRouters(int routerCount, UT::IPVersion ipVersion, int offset, int portCount) {
    m_offset = offset;
    m_routerCount = routerCount;
    m_routers.resize(m_routerCount);

    for (int id = 0; id < m_routerCount; ++id) {
        auto router = createRouter(id + 1 + m_offset, portCount, ipVersion);
        m_routers[id] = router;
    }
}

void TopologyBuilder::moveToMeshTopology(int rows, int columns) {
    resetBindings();

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            int id = i * columns + j;
            if (j > 0) bindPorts(m_routers[id]->getIdlePort(), m_routers[id - 1]->getIdlePort());
            if (i > 0) bindPorts(m_routers[id]->getIdlePort(), m_routers[id - columns]->getIdlePort());
        }
    }
}

void TopologyBuilder::moveToMeshTopology_(int rows, int columns) {
    resetBindings();

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            int id = i * columns + j;
            ///                         0 left      2 right
            if (j > 0) bindPorts(m_routers[id]->getIdlePort(), m_routers[id - 1]->getIdlePort());
            ///                         1 up        3 down
            if (i > 0) bindPorts(m_routers[id]->getIdlePort(), m_routers[id - columns]->getIdlePort());
        }
    }
}

void TopologyBuilder::moveToTorusTopology(int rows, int columns) {
    resetBindings();
    moveToMeshTopology(rows, columns);

    for (int i = 0; i < rows; ++i) {
        bindPorts(m_routers[i * columns]->getIdlePort(), m_routers[i * columns + columns - 1]->getIdlePort());
    }

    for (int j = 0; j < columns; ++j) {
        bindPorts(m_routers[j]->getIdlePort(), m_routers[(rows - 1) * columns + j]->getIdlePort());
    }
}

void TopologyBuilder::moveToStarTopology() {
    resetBindings();

    auto centerNode = m_routers[0];
    for (int i = 1; i <= m_routerCount; ++i) {
        auto node = m_routers[i];
        bindPorts(centerNode->getIdlePort(), node->getIdlePort());
    }
}

void TopologyBuilder::moveToRingStarTopology() {
    if (m_routerCount < 3) return;

    resetBindings();
    auto centerNode = m_routers.back();

    int i = 0;
    for (i = 0; i < m_routerCount - 1; ++i) {
        auto node = m_routers[i];
        if (i % 2 == 0) bindPorts(centerNode->getIdlePort(), node->getIdlePort());
        if (i > 0)      bindPorts(m_routers[i]->getIdlePort(), m_routers[i - 1]->getIdlePort());
    }
    bindPorts(m_routers[i-1]->getIdlePort(), m_routers[0]->getIdlePort());
}

bool TopologyBuilder::bindPorts(PortPtr_t port1, PortPtr_t port2) {
    if (port1 == nullptr || port1->state() != UT::PortState::Idle) {
        qDebug() << "Give port1 is not idle, cannot be bined.";
        return false;
    }
    if (port2 == nullptr || port2->state() != UT::PortState::Idle) {
        qDebug() << "Give port2 is not idle, cannot be bined.";
        return false;
    }

    m_portBindingManager.bind(port1, port2);
    return true;
}

bool TopologyBuilder::unbindPorts(PortPtr_t port1, PortPtr_t port2) {
    if (port1 == nullptr || port1->state() == UT::PortState::Idle) {
        qDebug() << "Give port1 is idle, cannot be unbined.";
        return false;
    }
    if (port2 == nullptr || port2->state() == UT::PortState::Idle) {
        qDebug() << "Give port2 is idle, cannot be unbined.";
        return false;
    }

    m_portBindingManager.bind(port1, port2);
    return true;
}

QVector<QSharedPointer<Router>> TopologyBuilder::routers() {
    return m_routers;
}
