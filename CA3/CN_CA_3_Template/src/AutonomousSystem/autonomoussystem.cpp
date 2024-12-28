#include "autonomoussystem.h"
#include "qjsonarray.h"

#include <QDebug>

AutonomousSystem::AutonomousSystem(int routerCount, int pcCount, int routerOffset, int pcOffset,
                                   UT::TopologyType topology_type, QObject *parent) :
    QObject(parent),
    m_routerCount(routerCount),
    m_pcCount(pcCount),
    m_routerOffset(routerOffset),
    m_pcOffset(pcOffset),
    m_isSimulationActive(false),
    m_topologyType(topology_type),
    m_ipVersion(UT::IPVersion::IPv4)
{
    int routerBufferSize = 6;
    m_topologyController = new TopologyController(routerBufferSize, this);
    initializeAS();
}

int AutonomousSystem::routerCount() {
    return m_routerCount;
}

int AutonomousSystem::pcCount() {
    return m_pcCount;
}

AutonomousSystem::~AutonomousSystem() {
    // delete m_dhcpServer;
    /// Note that this is QSharedPointer, will it interrupt the programm?????
    delete m_topologyController;
    // delete m_routingProtocol;
}

QSharedPointer<Router> AutonomousSystem::findRouterById(const int routerId) {
    for (const auto &router : m_routers) {
        if (router->id() == routerId) {
            return router;
        }
    }
    return nullptr;
}

void AutonomousSystem::setDHCPServer(int routerId) {
    auto router = findRouterById(routerId);
    if (router == nullptr) {
        qWarning() << "Router ID" << routerId << "not found in Autonomous System. Unable to set DHCP server.";
        return;
    }
    m_dhcpServer = router;
}

void AutonomousSystem::setUserGateways(const QJsonArray &userGateways) {
    m_userGateways.clear();
    for (const QJsonValue &value : userGateways) {
        auto router = findRouterById(0);
        m_userGateways.append(router);
    }
}

void AutonomousSystem::setBrokenRouters(const QJsonArray &brokenRouters) {
    m_brokenRouters.clear();
    for (const QJsonValue &value : brokenRouters) {
        auto router = findRouterById(0);
        m_brokenRouters.append(router);
    }
}

void AutonomousSystem::setGateways(const QJsonArray &gateways) {
    m_as_gateways.clear();
    for (const QJsonValue &value : gateways) {
        auto router = findRouterById(0);
        m_as_gateways.append(router);
    }
}

// void AutonomousSystem::setUserGateways(QJsonArray userGateways) {
//     m_userGateways = userGateways;
// }

// void AutonomousSystem::setBrokenRouters(QJsonArray brokenRouters) {
//     m_brokenRouters = brokenRouters;
// }

// void AutonomousSystem::setGateways(QJsonArray gateways) {
//     m_as_gateways = gateways;
// }

void AutonomousSystem::initializeAS() {
    int portCount = 4;
    m_topologyController->initializeTopology(m_topologyType, m_routerCount, m_ipVersion, m_routerOffset, portCount);
    m_routers = m_topologyController->getCurrentTopology();
    m_topologyController->getPcs(m_pcCount, m_pcOffset, m_ipVersion, 1);
    assignIPAddresses();
}

void AutonomousSystem::startRoutingProtocol() { /// UT::RoutingProtocolType protocol As input
    Q_EMIT routingProtocolStarted();

    // if (protocol == UT::RoutingProtocolType::RIP) {
    //     m_routingProtocol = new RIPProtocol(this);
    // } else if (protocol == UT::RoutingProtocolType::OSPF) {
    //     m_routingProtocol = new OSPFProtocol(this);
    // }

    // if (m_routingProtocol) {
    //     m_routingProtocol->startProtocol();
    //     Q_EMIT routingProtocolStarted();
    // }
}

void AutonomousSystem::stopRoutingProtocol() {
    Q_EMIT routingProtocolStopped();

    // if (m_routingProtocol) {
    //     m_routingProtocol->stopProtocol();
    //     delete m_routingProtocol;
    //     m_routingProtocol = nullptr;
    //     Q_EMIT routingProtocolStopped();
    // }
}

void AutonomousSystem::assignIPAddresses() {
    // m_dhcpServer->assignIPs(m_routers);
}

void AutonomousSystem::monitorNetwork() {
    if (m_isSimulationActive) {
        qDebug() << "Monitoring network...";
        for (const auto &router : m_routers) {
            qDebug() << "Router ID:" << router->id() << "has IP:"; // << router->ipAddress();
        }
    }
}

void AutonomousSystem::resetAS() {
    // m_routers.clear();
    m_topologyController->resetTopology();
}

void AutonomousSystem::configAS(UT::TopologyType topology_type) {
    m_isSimulationActive = false;
    m_topologyType = topology_type;
    m_topologyController->setTopologyType(m_topologyType);
    m_topologyController->updateTopology();
}

void AutonomousSystem::startSimulation() {
    m_isSimulationActive = true;
    m_topologyController->activateNodes();
    qDebug() << "Simulation started.";
}

void AutonomousSystem::stopSimulation() {
    m_isSimulationActive = false;
    m_topologyController->deactivateNodes();
    qDebug() << "Simulation stopped.";
}

QVector<QSharedPointer<Router>> AutonomousSystem::routers() {
    return m_routers;
}
