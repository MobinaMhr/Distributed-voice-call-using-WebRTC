#include "autonomoussystem.h"

#include <QDebug>

AutonomousSystem::AutonomousSystem(int node_count, UT::TopologyType topology_type, QObject *parent) :
    QObject(parent),
    m_node_count(node_count),
    m_isSimulationActive(false),
    m_topologyType(topology_type),
    m_topologyController(new TopologyController(this))
{
    initializeAS();
}

AutonomousSystem::~AutonomousSystem() {
    // delete m_dhcpServer;
    delete m_topologyController;
    // delete m_routingProtocol;
}

void AutonomousSystem::initializeAS() {
    m_topologyController->initializeTopology(m_topologyType, m_node_count);
    m_routers = m_topologyController->getCurrentTopology();
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
    m_topologyController->changeTopologyTo(m_topologyType);
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
