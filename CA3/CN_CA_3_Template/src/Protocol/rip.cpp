#include "RIP.h"
#include <QDebug>

RIP::RIP(IPv4Ptr_t routerIp, QObject *parent):
    m_currentRouterIp(routerIp) {
    m_routingTable = new RoutingTable(this);
}

void RIP::run()
{
    IpPtr_t fakeDest = IPv4_t::createIpPtr("255.255.255.255", "255.255.255.255");
    QByteArray payload ;
    DataLinkHeader *dh = new DataLinkHeader(this->m_routerMacAddress, this->m_routerMacAddress);
    TCPHeader *th = new TCPHeader(BROADCAST_ON_ALL_PORTS, BROADCAST_ON_ALL_PORTS);
    Packet *hello = new Packet(UT::PacketType::Control, UT::PacketControlType::RIP,
                                       1, 0, 0, fakeDest, payload, *dh, *th, m_routerIpv4Header, m_routerIpv6Header,
                                       RIP_TTL);

    hello->storeStringInPayload(generateUpdatePayload(HELLO, {}, {}));
    /// set hello packet as update packet : DONE!!!
    /// receive others hello -> add the sender ip by the cost 1 to the routing table???
    /// send update packet -> send current ips and costs in the routing table !!!
    /// receive others update packet -> update routing table enties if other update cost + other cost < current cost!!!
    /// if the routing table wasnt updated after n update packets emit end signal
}

void RIP::handleRIPPacket(const PacketPtr_t &packet, const QSharedPointer<Port> &port)
{
    QJsonObject update = extractUpdatePayloadJson(packet->readStringFromPayload());
    QString type = update["type"].toString();
    QVector<QString> nodes = extractNodes(update);
    QVector<int> costs = extractCosts(update);
    if (type == HELLO)
        handleHello(packet, port);
    else


}

QString RIP::generateUpdatePayload(QString type, QVector<IpPtr_t> nodes, QVector<int> costs)
{
    QJsonObject jsonObject;
    jsonObject["type"] = type;

    QJsonArray nodesArray;
    for (const IpPtr_t& node : nodes) {
        if (node) {
            nodesArray.append(node->toString());
        }
    }
    jsonObject["nodes"] = nodesArray;

    QJsonArray costsArray;
    for (const auto& cost : costs) {
        costsArray.append(cost);
    }
    jsonObject["costs"] = costsArray;

    QJsonDocument jsonDoc(jsonObject);
    return QString(jsonDoc.toJson(QJsonDocument::Compact));
}

QJsonObject RIP::extractUpdatePayloadJson(const QString &jsonString)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());

    if (!jsonDoc.isObject()) {
        qWarning() << "Invalid JSON string";
        return *(new QJsonObject);
    }

    return jsonDoc.object();
}

QVector<QString> RIP::extractNodes(QJsonObject update)
{
    QVector<QString> nodes;
    QJsonArray nodesArray = update["nodes"].toArray();
    for (const QJsonValue& nodeValue : nodesArray) {
        nodes.append(nodeValue.toString());
    }
    qDebug() << "Nodes:" << nodes;
    return nodes;
}

QVector<int> RIP::extractCosts(QJsonObject update)
{
    QVector<int> costs;
    QJsonArray costsArray = update["costs"].toArray();
    for (const QJsonValue& costValue : costsArray) {
        costs.append(costValue.toInt());
    }
    qDebug() << "Costs:" << costs;
    return costs;
}

void RIP::handleHello(const PacketPtr_t &packet, const QSharedPointer<Port> &port)
{
    IpPtr_t nodeIP ;
    if(packet->ipVersion() == UT::IPVersion::IPv4)
        nodeIP = IPv4_t::createIpPtr(packet->ipv4Header().sourceIp(), DEFAULT_MASK);
    else
        nodeIP = IPv6_t::createIpPtr(packet->ipv6Header().sourceIp(), DEFAULT_IPV6_PREFIX_LENGTH);
    m_routingTable->addRoute(nodeIP, nodeIP, port, PROTOCOL, NEIGHBOR_COST);
}

void RIP::handleUpdate(const PacketPtr_t &packet, const QVector<QString> &nodes, const QVector<int> costs, const QSharedPointer<Port> &port)
{

}

void RIP::handleUpdate(const PacketPtr_t &packet, const QSharedPointer<Port> &port)
{

}

// RIP::RIP(Router* router, QObject* parent)
//     : QObject(parent), m_router(router) {
//     connect(router, &Router::receivePacket, this, [this](const PacketPtr_t& packet) {
//         if (packet->packetType() == UT::PacketType::Control &&
//            packet->controlType() == UT::PacketControlType::RIPUpdate) {
//             handleRIPPacket(packet);
//         }
//     });
// }

// void RIP::initiateRoutingUpdate() {
//     sendRoutingUpdate(); // PacketPtr_t packet
// }

// void RIP::sendRoutingUpdate() {
//     QJsonObject routingData = createRoutingData(m_router->m_routing_table);
//     QJsonDocument doc(routingData);
//     QByteArray payload = doc.toJson(QJsonDocument::Compact);

//     for (auto& neighbor : m_router->neighbors()) {
//         Packet* ripPacket = new Packet(
//           UT::PacketType::Control,
//           UT::PacketControlType::RIPUpdate,
//           m_router->id(),  // source
//           neighbor->id(),  // destination
//           0,               // sequence
//           neighbor->ip(),
//           payload,
//           DataLinkHeader(m_router->macAddress(), neighbor->macAddress()),
//           TCPHeader(520, 520));  // RIP uses port 520

//         PacketPtr_t ripPacketPtr(ripPacket);
//         emit m_router->sendPacket(ripPacketPtr, neighbor->getIdlePort()->number());
//     }
// }

// void RIP::handleRIPPacket(const PacketPtr_t& packet) {
//     QJsonDocument doc = QJsonDocument::fromJson(packet->payload());
//     QJsonObject routingData = doc.object();

//     IpPtr_t neighborIp = packet->sourceIP();
//     int neighborMetric = 1; // Assuming metric for direct neighbors is 1

//     updateRoutingTable(routingData, neighborIp, neighborMetric);
// }

// void RIP::updateRoutingTable(const QJsonObject& routingData, const IpPtr_t& neighborIp, int neighborMetric) {
//     for (auto it = routingData.begin(); it != routingData.end(); ++it) {
//         IpPtr_t destIp = IPv4_t::createIpPtr(it.key(), DEFAULT_MASK);
//         QJsonObject entry = it.value().toObject();

//         int newMetric = entry["metric"].toInt() + neighborMetric;
//         if (!m_router->m_routing_table->routeExists(destIp) ||
//            m_router->m_routing_table->getPort(destIp)->metric > newMetric) {

//             QSharedPointer<Port> port = m_router->getIdlePort();
//             m_router->addRoutingEntry(destIp, neighborIp, port, newMetric);
//         }
//     }
// }

// QJsonObject RIP::createRoutingData(const RoutingTable* routingTable) {
//     QJsonObject routingData;

//     for (const auto& route : routingTable->getAllRoutes()) {
//         QJsonObject entry;
//         entry["nextHop"] = route.nextHopIp->toString();
//         entry["metric"] = route.metric;
//         routingData[route.nextHopIp->toString()] = entry;
//     }

//     return routingData;
// }

// void RIP::onNeighborTimeout() {
//     for (auto it = m_router->neighbors().begin(); it != m_router->neighbors().end(); ++it) {
//         // Check if the neighbor has not responded
//         if (!(*it)->isAlive()) {
//             qDebug() << "Neighbor down:" << (*it)->name();
//             m_router->m_routing_table->removeRoute((*it)->ip());
//         }
//     }
// }
