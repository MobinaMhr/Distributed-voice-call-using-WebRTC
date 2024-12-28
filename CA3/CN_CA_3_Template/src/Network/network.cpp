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
    QString configFilePath_ = "./../../assets/config.json";
    loadConfig(configFilePath_);
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

    // QJsonArray asArray = m_config["Autonomous_systems"].toArray();
    // for (const auto &asValue : asArray) {
    //     QJsonObject asConfig = asValue.toObject();
    //     int id = asConfig["id"].toInt();
    //     qDebug() << "id: " << id;
    //     QString topologyType = asConfig["topology_type"].toString();
    //     qDebug() << "topologyType: " << topologyType;
    //     int nodeCount = asConfig["node_count"].toInt();
    //     qDebug() << "nodeCount: " << nodeCount;
    //     int routerOffset = id * 100; // Example offset calculation
    //     qDebug() << "routerOffset: " << routerOffset;
    //     int pcOffset = id * 1000;
    //     qDebug() << "pcOffset: " << pcOffset;
    // }
}


void Network::initializeNetwork() {
    QJsonArray asArray = m_config["Autonomous_systems"].toArray();
    for (const auto &asValue : asArray) {
        QJsonObject asConfig = asValue.toObject();
        int id = asConfig["id"].toInt();
        QString topologyType = asConfig["topology_type"].toString();
        int nodeCount = asConfig["node_count"].toInt();
        int routerOffset = id * 100; // Example offset calculation
        int pcOffset = id * 1000;
        UT::TopologyType type = (topologyType == "Mesh") ? UT::TopologyType::Mesh : UT::TopologyType::Torus;

        m_autonomousSystems.append(QSharedPointer<AutonomousSystem>::create(nodeCount, 0, routerOffset, pcOffset, type));
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
