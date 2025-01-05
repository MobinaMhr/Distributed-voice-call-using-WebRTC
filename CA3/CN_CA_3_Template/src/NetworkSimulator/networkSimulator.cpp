#include "networkSimulator.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

NetworkSimulator::NetworkSimulator(const QString &configFilePath, QObject *parent)
    : QObject(parent),
    m_routingCompletionCount(0),
    m_totalRouters(0) {}

NetworkSimulator::~NetworkSimulator() {
    delete m_eventCoordinator;
}

void NetworkSimulator::loadConfig(const QString &filePath) {
    QFile configFile(filePath);

    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open configuration file.";
        return;
    }

    QByteArray configData = configFile.readAll();
    QJsonDocument configDoc = QJsonDocument::fromJson(configData);
    m_config = configDoc.object();
    configFile.close();
}

std::pair<int, int> NetworkSimulator::calculateOffsets() {
    int routerOffset = m_autonomousSystems.isEmpty() ? 0 : m_autonomousSystems.back()->routerCount();
    int pcOffset = m_autonomousSystems.isEmpty() ? 23 : 23 + m_autonomousSystems.back()->pcCount();
    return std::make_pair(routerOffset, pcOffset);
}

UT::TopologyType NetworkSimulator::returnTopologyType(QString topologyType) {
    return (topologyType == "Mesh") ? UT::TopologyType::Mesh : UT::TopologyType::Torus;
}

void NetworkSimulator::generateEventCoordinator(double lambda, int cycleCount, int packetCount, int pcCount, int cycleLength) {
    m_eventCoordinator = EventsCoordinator::instance(lambda, cycleCount, packetCount, pcCount, cycleLength);
}

void NetworkSimulator::initializeNetwork() {
    QString simulation_duration = m_config["simulation_duration"].toString();
    QString cycle_duration = m_config["cycle_duration"].toString();
    int ttl = m_config["TTL"].toInt();
    /// TODO:: send ttl to Data Packet.
    int packets_per_simulation = m_config["packets_per_simulation"].toInt();
    int router_buffer_size = m_config["router_buffer_size"].toInt();
    int router_port_count = m_config["router_port_count"].toInt();
    QString routing_protocol = m_config["routing_protocol"].toString();
    QString routing_table_update_interval = m_config["routing_table_update_interval"].toString();
    // bool routing_per_port = m_config["routing_per_port"].toBool();
    // int routing_table_size = m_config["routing_table_size"].toInt();
    // int routing_packets_per_port_cycle = m_config["routing_packets_per_port_cycle"].toInt();

    QJsonArray asArray = m_config["Autonomous_systems"].toArray();
    for (const auto &asValue : asArray) {
        QJsonObject asConfig = asValue.toObject();
        int id = asConfig["id"].toInt();
        UT::TopologyType type = returnTopologyType(asConfig["topology_type"].toString());
        int routerCount = asConfig["node_count"].toInt();

        auto offsets = calculateOffsets();
        int routerOffset = offsets.first;
        int pcOffset = offsets.second;

        QSharedPointer<AutonomousSystem> asInstance;
        asInstance = QSharedPointer<AutonomousSystem>::create(
            id, routerCount, 0, routerOffset, pcOffset, type, router_buffer_size, router_port_count
        );

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
        m_totalRouters += routerCount;
        // Configure gateways and inter-AS connections here...
    }

    int pcCount = 15;
    int cycleCount = 5;
    int cycleLength = 100;

    connectAS();
    generateEventCoordinator(
        10.0, cycleCount, packets_per_simulation, pcCount, cycleLength
    );
}

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

void NetworkSimulator::startPhaseOne(const QString &configFilePath) {
    qDebug() << "Starting Phase One: NetworkSimulator Construction.";
    loadConfig(configFilePath);
    initializeNetwork();
}

void NetworkSimulator::startPhaseTwo() {
    qDebug() << "Starting Phase Two: Identification.";
    for (const auto &as : m_autonomousSystems) {
        as->startRoutingProtocol();
    }

    monitorRoutingCompletion();
}

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

void NetworkSimulator::startPhaseThree() {
    qDebug() << "Starting Phase Three: Execution.";
    m_config["simulation_duration"].toString();
}

void NetworkSimulator::startPhaseFour() {
    qDebug() << "Starting Phase Four: Analysis.";
    analyzeResults();
}

void NetworkSimulator::analyzeResults() {
    qDebug() << "Analyzing simulation results...";
}

// void NetworkSimulator::resetNetwork() {
//     qDebug() << "Resetting network...";
//     m_autonomousSystems.clear();
//     initializeNetwork();
// }

// void NetworkSimulator::cleanLogs() {
//     qDebug() << "Cleaning logs...";
//     // Implement log deletion logic.
// }

// void NetworkSimulator::printRoutingTable(int nodeId) {
//     for (const auto &as : m_autonomousSystems) {
//         for (const auto &router : as->routers()) {
//             if (router->id() == nodeId) {
//                 router->m_routing_table->printRoutingTable();
//                 return;
//             }
//         }
//     }
//     qWarning() << "Node ID" << nodeId << "not found in any Autonomous System.";
// }

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

