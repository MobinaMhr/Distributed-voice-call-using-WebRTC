#include "autonomoussystem.h"

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

AutonomousSystem::~AutonomousSystem() {
    // delete m_dhcpServer;
    delete m_topologyController;
    // delete m_routingProtocol;
}

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
