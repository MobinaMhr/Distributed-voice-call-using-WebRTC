#include "RIP.h"
#include <QDebug>

RIP::RIP(IPv4Ptr_t routerIp, MacAddress routerMac, QObject* parent)
    : QObject(parent),
    m_currentRouterIp(routerIp),
    m_updatePacket(UT::PacketType::Control, UT::PacketControlType::RIP, 1, 0, 0,
                   IPv4_t::createIpPtr("0.0.0.0", "255.255.255.0"), QByteArray(),
                   DataLinkHeader(MacAddress("00:00:00:00:00:00"), MacAddress("00:00:00:00:00:00")),
                   TCPHeader(0, 0), IPHv4_t(), IPHv6_t(), RIP_TTL, this),
    m_routerMacAddress(routerMac), m_updateIsReady(false), m_isFinished(false), m_notUpdatedTimes(0)
{
    m_routingTable = new RoutingTable(this);
    m_routerIpv4Header = *(new IPHv4_t());
    m_routerIpv4Header.setSourceIp(m_currentRouterIp);
    m_routerIpv6Header = *(new IPHv6_t());
    m_routerIpv6Header.setSourceIp(m_currentRouterIp->toIPv6());
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

    QVector<IpPtr_t> nodes = {};

    hello->storeStringInPayload(generateUpdatePayload(HELLO, nodes, {}));
    m_updatePacket = *hello;
    m_updateIsReady = true;
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
        handleUpdate(packet, nodes, costs, port);


}

bool RIP::isUpdateReady()
{
    return m_updateIsReady;
}

Packet RIP::getUpdatePacket()
{
    if(!m_isFinished)
        m_updateIsReady = false;
    return m_updatePacket;
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

QString RIP::generateUpdatePayload(QString type, QVector<QString> nodes, QVector<int> costs)
{
    QJsonObject jsonObject;
    jsonObject["type"] = type;

    QJsonArray nodesArray;
    for (const QString& node : nodes)
            nodesArray.append(node);

    jsonObject["nodes"] = nodesArray;

    QJsonArray costsArray;
    for (const auto& cost : costs)
        costsArray.append(cost);
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
    IpPtr_t nodeIP;
    if(packet->ipVersion() == UT::IPVersion::IPv4)
        nodeIP = IPv4_t::createIpPtr(packet->ipv4Header().sourceIp(), DEFAULT_MASK);
    else
        nodeIP = IPv6_t::createIpPtr(packet->ipv6Header().sourceIp(), DEFAULT_IPV6_PREFIX_LENGTH);
    m_routingTable->addRoute(nodeIP, nodeIP, port, PROTOCOL, NEIGHBOR_COST);
    generateUpdatePacket();
    m_updateIsReady = true;
}

void RIP::handleUpdate(const PacketPtr_t &packet, const QVector<QString> &nodes, const QVector<int> costs, const QSharedPointer<Port> &port)
{
    IpPtr_t nextHopIP;
    bool isUpdated = false;
    if(packet->ipVersion() == UT::IPVersion::IPv4)
        nextHopIP = IPv4_t::createIpPtr(packet->ipv4Header().sourceIp(), DEFAULT_MASK);
    else
        nextHopIP = IPv6_t::createIpPtr(packet->ipv6Header().sourceIp(), DEFAULT_IPV6_PREFIX_LENGTH);
    for (int i = 0; i<nodes.size(); i++){
        auto node = nodes[i];
        IpPtr_t nodeIP;
        if (node.contains(':'))
            nodeIP = IPv6_t::createIpPtr(node, DEFAULT_IPV6_PREFIX_LENGTH);
        else
            nodeIP = IPv4_t::createIpPtr(node, DEFAULT_MASK);
        int currentCost = m_routingTable->getRouteCost(nodeIP);
        int costFromNeighbor = costs[i] + NEIGHBOR_COST;
        if (currentCost > costFromNeighbor){
            m_routingTable->updateRoute(nodeIP ,nextHopIP, port, costFromNeighbor);
            isUpdated = true;
        }
    }
    if (!isUpdated)
        m_notUpdatedTimes++;
    else
        m_updateIsReady = true;
    if (m_notUpdatedTimes >= FINISH_THRESHOLD){
        m_isFinished = true;
        Q_EMIT routingFinished(*m_routingTable);
    }
    generateUpdatePacket();
}

QString RIP::generateUpdatePacket()
{
    IpPtr_t fakeDest = IPv4_t::createIpPtr("255.255.255.255", "255.255.255.255");
    QByteArray payload ;
    DataLinkHeader *dh = new DataLinkHeader(this->m_routerMacAddress, this->m_routerMacAddress);
    TCPHeader *th = new TCPHeader(BROADCAST_ON_ALL_PORTS, BROADCAST_ON_ALL_PORTS);
    Packet *update = new Packet(UT::PacketType::Control, UT::PacketControlType::RIP,
                                       1, 0, 0, fakeDest, payload, *dh, *th, m_routerIpv4Header, m_routerIpv6Header,
                                       RIP_TTL);
    auto nodes = m_routingTable->getNodes();
    auto costs = m_routingTable->getCosts();

    update->storeStringInPayload(generateUpdatePayload(UPDATE, nodes, costs));
    m_updatePacket = *update;
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
RIP::RIP::RIP(const RIP &other) :
    QObject(other.parent()),
    m_currentRouterIp(other.m_currentRouterIp),
    m_updatePacket(other.m_updatePacket),
    m_routerMacAddress(other.m_routerMacAddress),
    m_routerIpv4Header(other.m_routerIpv4Header),
    m_routerIpv6Header(other.m_routerIpv6Header),
    m_updateIsReady(other.m_updateIsReady),
    m_isFinished(other.m_isFinished),
    m_notUpdatedTimes(other.m_notUpdatedTimes)
{
    m_routingTable = new RoutingTable(*other.m_routingTable);
}

RIP &
RIP::RIP::operator=(const RIP &other)
{
    if(this != &other)
    {
        QObject::setParent(other.parent());

        // Clean up existing resources
        delete m_routingTable;

        m_currentRouterIp  = other.m_currentRouterIp;
        m_updatePacket     = other.m_updatePacket;
        m_routerMacAddress = other.m_routerMacAddress;
        m_routerIpv4Header = other.m_routerIpv4Header;
        m_routerIpv6Header = other.m_routerIpv6Header;
        m_updateIsReady    = other.m_updateIsReady;
        m_isFinished       = other.m_isFinished;
        m_notUpdatedTimes  = other.m_notUpdatedTimes;

        // Perform a deep copy of the RoutingTable
        m_routingTable     = new RoutingTable(*other.m_routingTable);
    }
    return *this;
}
