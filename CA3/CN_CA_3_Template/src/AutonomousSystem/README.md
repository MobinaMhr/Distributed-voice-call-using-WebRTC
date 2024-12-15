# AutonomousSystem

The `AutonomousSystem` class is designed to manage an autonomous system (AS) in a network simulation or application. It provides functionalities for initializing the AS, starting and stopping routing protocols, assigning IP addresses, monitoring the network, and starting and stopping simulations. The class interacts with the TopologyController to create and configure network nodes and topologies.

```cpp
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
```

The constructor sets some values to its private attributes.
It also creates an instance of TopologyController.
Then it calls the `initialzeAS()` method.

```cpp
AutonomousSystem::~AutonomousSystem() {
    delete m_topologyController;
}
```

The destructor method deletes all created pointers.

```cpp
void AutonomousSystem::initializeAS() {
    int portCount = 4;
    m_topologyController->initializeTopology(m_topologyType, m_routerCount, m_ipVersion, m_routerOffset, portCount);
    m_routers = m_topologyController->getCurrentTopology();

    m_topologyController->getPcs(m_pcCount, m_pcOffset, m_ipVersion, 1);

    // assignIPAddresses();
}
```

The `initialzieAS()` method initializes the topology and gets it. It also creates all PCs and makes the topology controller connect them.

```cpp
void AutonomousSystem::startRoutingProtocol() { /// UT::RoutingProtocolType protocol As input
    Q_EMIT routingProtocolStarted();
}
```

```cpp
void AutonomousSystem::stopRoutingProtocol() {
    Q_EMIT routingProtocolStopped();
}
```

```cpp
void AutonomousSystem::monitorNetwork() {
    if (m_isSimulationActive) {
        qDebug() << "Monitoring network...";
        for (const auto &router : m_routers) {
            qDebug() << "Router ID:" << router->id() << "has IP:"; // << router->ipAddress();
        }
    }
}
```

```cpp
void AutonomousSystem::resetAS() {
    // m_routers.clear();
    m_topologyController->resetTopology();
}
```

The reset AS will reset the created topology.

```cpp
void AutonomousSystem::configAS(UT::TopologyType topology_type) {
    m_isSimulationActive = false;
    m_topologyType = topology_type;
    m_topologyController->setTopologyType(m_topologyType);
    m_topologyController->updateTopology();
}
```

The `configAS()` configs the topology in the AS.

```cpp
void AutonomousSystem::startSimulation() {
    m_isSimulationActive = true;
    m_topologyController->activateNodes();
    qDebug() << "Simulation started.";
}
```

The start simulates in AS will activate all nodes in the topology.

```cpp
void AutonomousSystem::stopSimulation() {
    m_isSimulationActive = false;
    m_topologyController->deactivateNodes();
    qDebug() << "Simulation stopped.";
}
```

The stop simulates in AS will activate all nodes in the topology.