#include "network.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

Network::Network(const QString &configFilePath, QObject *parent)
    : QObject(parent),
    m_eventCoordinator(EventsCoordinator::instance(1.0, 10, 100, 5, 1000)),
    m_routingCompletionCount(0),
    m_totalRouters(0) {
    loadConfig(configFilePath);
    initializeNetwork();
}

Network::~Network() {
    delete m_eventCoordinator;
}

void Network::loadConfig(const QString &filePath) {
    QFile configFile(filePath);
    qDebug() << "Current working directory:" << QDir::currentPath();
    qDebug() << "Attempting to open configuration file at:" << QFileInfo(configFile).absoluteFilePath();

    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open configuration file.";
        return;
    }

    QByteArray configData = configFile.readAll();
    QJsonDocument configDoc = QJsonDocument::fromJson(configData);
    m_config = configDoc.object();
    configFile.close();
}

void Network::isConfigLoaded() {
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

void Network::initializeNetwork() {
    QJsonArray asArray = m_config["Autonomous_systems"].toArray();
    for (const auto &asValue : asArray) {
        QJsonObject asConfig = asValue.toObject();
        int id = asConfig["id"].toInt();
        qDebug() << "id: " << id;
        QString topologyType = asConfig["topology_type"].toString();
        qDebug() << "topologyType: " << topologyType;
        int nodeCount = asConfig["node_count"].toInt();
        qDebug() << "nodeCount: " << nodeCount;

        int routerOffset = id * 100; // Example offset calculation
        qDebug() << "routerOffset: " << routerOffset;
        int pcOffset = id * 1000;
        qDebug() << "pcOffset: " << pcOffset;
        UT::TopologyType type = (topologyType == "Mesh") ? UT::TopologyType::Mesh : UT::TopologyType::Torus;

        QSharedPointer<AutonomousSystem> asInstance;
        asInstance = QSharedPointer<AutonomousSystem>::create(nodeCount, 0, routerOffset, pcOffset, type);

        int dhcpServer = asConfig["dhcp_server"].toInt();
        asInstance->setDHCPServer(dhcpServer);

        // QJsonArray userGateways = asConfig["user_gateways"].toArray();
        // asInstance->setUserGateways(userGateways);
        // QJsonArray brokenRouters = asConfig["broken_routers"].toArray();
        // asInstance->setBrokenRouters(brokenRouters);
        // QJsonArray gateways = asConfig["gateways"].toArray();
        // asInstance->setGateways(gateways);
        // QJsonValue connectToAS = asConfig["connect_to_as"];
        /// TODO::::::::::::::::::::::::::


        m_autonomousSystems.append(asInstance);
        m_totalRouters += nodeCount;
        // Configure gateways and inter-AS connections here...
    }

    connectAS();
}

void Network::connectAS() {
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

// void Network::startPhaseOne() {
//     qDebug() << "Starting Phase One: Network Construction.";
//     // for (const auto &as : m_autonomousSystems) {
//     //     as->initializeAS();
//     // }
// }

// void Network::startPhaseTwo() {
//     qDebug() << "Starting Phase Two: Identification.";
//     // for (const auto &as : m_autonomousSystems) {
//     //     as->startRoutingProtocol();
//     // }

//     // monitorRoutingCompletion();
// }

// void Network::monitorRoutingCompletion() {
//     for (const auto &as : m_autonomousSystems) {
//         connect(as.data(), &AutonomousSystem::routingProtocolStopped, this, [this]() {
//             m_routingCompletionCount++;
//             if (m_routingCompletionCount == m_totalRouters) {
//                 qDebug() << "All routing protocols have completed.";
//                 // Proceed to the next phase or mark as completed.
//             }
//         });
//     }
// }

// void Network::startPhaseThree() {
//     qDebug() << "Starting Phase Three: Execution.";
//     // m_eventCoordinator->startExecution(m_config["simulation_duration"].toString());
// }

// void Network::startPhaseFour() {
//     qDebug() << "Starting Phase Four: Analysis.";
//     // analyzeResults();
// }

// void Network::analyzeResults() {
//     // Example analysis: packet-loss, hop-count-avg, etc.
//     qDebug() << "Analyzing simulation results...";
//     // Perform analysis and display results.
// }

// void Network::resetNetwork() {
//     qDebug() << "Resetting network...";
//     m_autonomousSystems.clear();
//     initializeNetwork();
// }

// void Network::cleanLogs() {
//     qDebug() << "Cleaning logs...";
//     // Implement log deletion logic.
// }

// void Network::printRoutingTable(int nodeId) {
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
