# NetworkSimulator

The constructor of this class initializes the NetworkSimulator object with the given configuration file path and parent object.

Parameters:

- configFilePath: Path to the configuration file.
- parent: Parent object.

Initialization:

- It calls the base class QObject constructor with the parent object.
- It initializes m_eventCoordinator with an instance of EventsCoordinator.
- It sets m_routingCompletionCount and m_totalRouters to 0.

```cpp
NetworkSimulator::NetworkSimulator(const QString &configFilePath, QObject *parent)
    : QObject(parent),
    m_eventCoordinator(EventsCoordinator::instance(1.0, 10, 100, 5, 1000)),
    m_routingCompletionCount(0),
    m_totalRouters(0) {}
```

It cleans up resources used by the NetworkSimulator object and deletes the m_eventCoordinator object.

```cpp
NetworkSimulator::~NetworkSimulator() {
    delete m_eventCoordinator;
}
```

It loads the network configuration from the specified file.
It opens the configuration file for reading.
It reads the file content and parses it as a JSON document.
It stores the parsed JSON object in m_config.

```cpp
void NetworkSimulator::loadConfig(const QString &filePath) {
    QFile configFile(filePath);
    // qDebug() << "Current working directory:" << QDir::currentPath();
    // qDebug() << "Attempting to open configuration file at:" << QFileInfo(configFile).absoluteFilePath();

    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open configuration file.";
        return;
    }

    QByteArray configData = configFile.readAll();
    QJsonDocument configDoc = QJsonDocument::fromJson(configData);
    m_config = configDoc.object();
    configFile.close();
}
```

It checks if the configuration is loaded and extracts basic simulation parameters.
It checks if m_config is not empty.
It logs the status of the configuration.
It extracts and logs various simulation parameters from m_config.
It processes and logs details of autonomous systems defined in the configuration.

```cpp
void NetworkSimulator::isConfigLoaded() {
    bool isLoaded = !m_config.isEmpty();
    qDebug() << "isLoaded: " << isLoaded;

    if (!isLoaded) {
        qWarning() << "Configuration is empty.";
        return;
    }

    // Extract basic simulation parameters
    qDebug() << "Simulation Duration:" << m_config["simulation_duration"].toString();
    qDebug() << "Cycle Duration:" << m_config["cycle_duration"].toString();
    qDebug() << "TTL:" << m_config["TTL"].toInt();
    qDebug() << "Packets Per Simulation:" << m_config["packets_per_simulation"].toInt();
    qDebug() << "Statistical Distribution:" << m_config["statistical_distribution"].toString();
    qDebug() << "Router Buffer Size:" << m_config["router_buffer_size"].toInt();
    qDebug() << "Router Port Count:" << m_config["router_port_count"].toInt();
    qDebug() << "Routing Protocol:" << m_config["routing_protocol"].toString();
    qDebug() << "Routing Table Update Interval:" << m_config["routing_table_update_interval"].toString();
    qDebug() << "Routing Per Port:" << m_config["routing_per_port"].toBool();
    qDebug() << "Routing Table Size:" << m_config["routing_table_size"].toInt();
    qDebug() << "Routing Packets Per Port Cycle:" << m_config["routing_packets_per_port_cycle"].toInt();

    // Process Autonomous Systems
    QJsonArray asArray = m_config["Autonomous_systems"].toArray();
    for (const auto &asValue : asArray) {
        QJsonObject asConfig = asValue.toObject();
        int id = asConfig["id"].toInt();
        QString topologyType = asConfig["topology_type"].toString();
        int nodeCount = asConfig["node_count"].toInt();
        QJsonArray asGateways = asConfig["as_gateways"].toArray();
        QJsonArray userGateways = asConfig["user_gateways"].toArray();
        int dhcpServer = asConfig["dhcp_server"].toInt();
        QJsonArray brokenRouters = asConfig["broken_routers"].toArray();
        QJsonArray gateways = asConfig["gateways"].toArray();
        QJsonValue connectToAS = asConfig["connect_to_as"];

        qDebug() << "\n--- Autonomous System ID:" << id << "---";
        qDebug() << "Topology Type:" << topologyType;
        qDebug() << "Node Count:" << nodeCount;
        qDebug() << "AS Gateways:" << asGateways;
        qDebug() << "User Gateways:" << userGateways;
        qDebug() << "DHCP Server ID:" << dhcpServer;
        qDebug() << "Broken Routers:" << brokenRouters;

        qDebug() << "--- Gateways ---";
        for (const auto &gateway : gateways) {
            QJsonObject gatewayObj = gateway.toObject();
            int gatewayNode = gatewayObj["node"].toInt();
            QJsonArray users = gatewayObj["users"].toArray();
            qDebug() << "Gateway Node:" << gatewayNode << "Users:" << users;
        }

        if (!connectToAS.isNull()) {
            qDebug() << "--- Connect to AS ---";
            QJsonArray connections = connectToAS.toArray();
            for (const auto &connection : connections) {
                QJsonObject connectionObj = connection.toObject();
                int connectedASId = connectionObj["id"].toInt();
                QJsonArray gatewayPairs = connectionObj["gateway_pairs"].toArray();

                qDebug() << "Connect to AS ID:" << connectedASId;
                for (const auto &pair : gatewayPairs) {
                    QJsonObject pairObj = pair.toObject();
                    int gateway = pairObj["gateway"].toInt();
                    int connectTo = pairObj["connect_to"].toInt();
                    qDebug() << "Gateway:" << gateway << "Connect To:" << connectTo;
                }
            }
        }
    }
}
```

It calculates offsets for routers and PCs based on the current state of autonomous systems.
It returns a pair of integers representing the router and PC offsets.

```cpp
std::pair<int, int> NetworkSimulator::calculateOffsets() {
    int routerOffset = m_autonomousSystems.isEmpty() ? 0 : m_autonomousSystems.back()->routerCount();
    int pcOffset = m_autonomousSystems.isEmpty() ? 23 : 23 + m_autonomousSystems.back()->pcCount();
    return std::make_pair(routerOffset, pcOffset);
}
```

It initializes the network based on the loaded configuration.
It extracts simulation parameters from m_config.
It iterates through autonomous systems defined in the configuration.
It creates instances of AutonomousSystem and sets various parameters.
It appends the created instances to m_autonomousSystems.
It updates the total router count.
It calls connectAS to establish connections between autonomous systems.

```cpp
void NetworkSimulator::initializeNetwork() {
    QString simulation_duration = m_config["simulation_duration"].toString();
    QString cycle_duration = m_config["cycle_duration"].toString();
    int ttl = m_config["TTL"].toInt();
    int packets_per_simulation = m_config["packets_per_simulation"].toInt();
    QString statistical_distribution = m_config["statistical_distribution"].toString();
    int router_buffer_size = m_config["router_buffer_size"].toInt();
    int router_port_count = m_config["router_port_count"].toInt();
    QString routing_protocol = m_config["routing_protocol"].toString();
    QString routing_table_update_interval = m_config["routing_table_update_interval"].toString();
    bool routing_per_port = m_config["routing_per_port"].toBool();
    int routing_table_size = m_config["routing_table_size"].toInt();
    int routing_packets_per_port_cycle = m_config["routing_packets_per_port_cycle"].toInt();

    QJsonArray asArray = m_config["Autonomous_systems"].toArray();
    for (const auto &asValue : asArray) {
        QJsonObject asConfig = asValue.toObject();
        int id = asConfig["id"].toInt();
        qDebug() << "id: " << id;
        QString topologyType = asConfig["topology_type"].toString();
        UT::TopologyType type = (topologyType == "Mesh") ? UT::TopologyType::Mesh : UT::TopologyType::Torus;
        qDebug() << "topologyType: " << topologyType;
        int nodeCount = asConfig["node_count"].toInt();
        qDebug() << "nodeCount: " << nodeCount;

        auto offsets = calculateOffsets();
        int routerOffset = offsets.first;
        qDebug() << "routerOffset: " << routerOffset;
        int pcOffset = offsets.second;
        qDebug() << "pcOffset: " << pcOffset;

        QSharedPointer<AutonomousSystem> asInstance;
        asInstance = QSharedPointer<AutonomousSystem>::create(nodeCount, 0, routerOffset, pcOffset, type);

        int dhcpServer = asConfig["dhcp_server"].toInt();
        asInstance->setDHCPServer(dhcpServer);

        QJsonArray userGateways = asConfig["user_gateways"].toArray();
        asInstance->setUserGateways(userGateways);
        QJsonArray brokenRouters = asConfig["broken_routers"].toArray();
        asInstance->setBrokenRouters(brokenRouters);
        QJsonArray gateways = asConfig["gateways"].toArray();
        asInstance->setGateways(gateways);

        /// TODO
        QJsonArray asGateways = asConfig["as_gateways"].toArray();
        QJsonValue connectToAS = asConfig["connect_to_as"];
        for (const auto &gateway : gateways) {
            QJsonObject gatewayObj = gateway.toObject();
            int gatewayNode = gatewayObj["node"].toInt();
            QJsonArray users = gatewayObj["users"].toArray();
            qDebug() << "Gateway Node:" << gatewayNode << "Users:" << users;
            /// TODO::
        }
        if (!connectToAS.isNull()) {
            qDebug() << "--- Connect to AS ---";
            QJsonArray connections = connectToAS.toArray();
            for (const auto &connection : connections) {
                QJsonObject connectionObj = connection.toObject();
                int connectedASId = connectionObj["id"].toInt();
                QJsonArray gatewayPairs = connectionObj["gateway_pairs"].toArray();

                qDebug() << "Connect to AS ID:" << connectedASId;
                for (const auto &pair : gatewayPairs) {
                    QJsonObject pairObj = pair.toObject();
                    int gateway = pairObj["gateway"].toInt();
                    int connectTo = pairObj["connect_to"].toInt();
                    qDebug() << "Gateway:" << gateway << "Connect To:" << connectTo;
                    /// TODO::
                }
            }
        }
        /// END TODO

        m_autonomousSystems.append(asInstance);
        m_totalRouters += nodeCount;
        // Configure gateways and inter-AS connections here...
    }

    connectAS();
}
```

It establishes connections between autonomous systems based on the configuration.
It iterates through autonomous systems and their connections.
It establishes connections between source and target gateways.

```cpp
void NetworkSimulator::connectAS() {
    QJsonArray asArray = m_config["Autonomous_systems"].toArray();
    for (const auto &asValue : asArray) {
        QJsonObject asConfig = asValue.toObject();
        QJsonArray connections = asConfig["connect_to_as"].toArray();

        for (const auto &connection : connections) {
            QJsonObject connectionObj = connection.toObject();
            int targetASId = connectionObj["id"].toInt();
            QJsonArray gatewayPairs = connectionObj["gateway_pairs"].toArray();

            for (const auto &pair : gatewayPairs) {
                QJsonObject pairObj = pair.toObject();
                int sourceGateway = pairObj["gateway"].toInt();
                int targetGateway = pairObj["connect_to"].toInt();

                // Establish connections between sourceGateway and targetGateway
                // through their respective AutonomousSystems.
            }
        }
    }
}
```

IT starts the first phase of the network simulation (NetworkSimulator Construction).
It logs the start of Phase One.
It calls loadConfig to load the configuration.
It calls initializeNetwork to set up the network.

```cpp
void NetworkSimulator::startPhaseOne(const QString &configFilePath) {
    qDebug() << "Starting Phase One: NetworkSimulator Construction.";
    loadConfig(configFilePath);
    initializeNetwork();
}
```

It starts the second phase of the network simulation (Identification).
It logs the start of Phase Two.
It starts the routing protocol for each autonomous system.
It calls monitorRoutingCompletion to monitor the completion of routing protocols.

```cpp
void NetworkSimulator::startPhaseTwo() {
    qDebug() << "Starting Phase Two: Identification.";
    for (const auto &as : m_autonomousSystems) {
        as->startRoutingProtocol();
    }

    monitorRoutingCompletion();
}
```

It monitors the completion of routing protocols for all autonomous systems.
It connects to the routingProtocolStopped signal of each autonomous system.
It increments m_routingCompletionCount when a routing protocol stops.
It logs the completion of all routing protocols when the count matches m_totalRouters.

```cpp
void NetworkSimulator::monitorRoutingCompletion() {
    for (const auto &as : m_autonomousSystems) {
        connect(as.data(), &AutonomousSystem::routingProtocolStopped, this, [this]() {
            m_routingCompletionCount++;
            if (m_routingCompletionCount == m_totalRouters) {
                qDebug() << "All routing protocols have completed.";
                return;
            }
        });
    }
}
```

It starts the third phase of the network simulation (Execution).
IT logs the start of Phase Three. And also uses the simulation duration from m_config.

```cpp
void NetworkSimulator::startPhaseThree() {
    qDebug() << "Starting Phase Three: Execution.";
    m_config["simulation_duration"].toString();
}
```

This method starts the fourth phase of the network simulation (Analysis).
It logs the start of Phase Four. And also it calls analyzeResults to analyze the simulation results.

```cpp
void NetworkSimulator::startPhaseFour() {
    qDebug() << "Starting Phase Four: Analysis.";
    analyzeResults();
}
```

This method analyzes the results of the network simulation.
It logs the start of result analysis.

```cpp
void NetworkSimulator::analyzeResults() {
    qDebug() << "Analyzing simulation results...";
}
```
