#include "ospf.h"
#include <QDebug>

OSPF::OSPF(IPv4Ptr_t routerIp, MacAddress routerMac, QObject* parent)
    : QObject(parent),
    m_routerIp(routerIp),
    m_lsaPacket(UT::PacketType::Control, UT::PacketControlType::OSPF, 1, 0, 0,
                   IPv4_t::createIpPtr("0.0.0.0", "255.255.255.0"), QByteArray(),
                   DataLinkHeader(MacAddress("00:00:00:00:00:00"), MacAddress("00:00:00:00:00:00")),
                   TCPHeader(0, 0), IPHv4_t(), IPHv6_t(), OSPF_TTL, this),
    m_routerMac(routerMac),
    m_lsaIsReady(false),
    m_isFinished(false),
    m_notUpdatedTimes(0) {
    m_routingTable = new RoutingTable(this);
    m_routerIpv4Header = *(new IPHv4_t());
    m_routerIpv4Header.setSourceIp(m_routerIp);
    m_routerIpv6Header = *(new IPHv6_t());
    m_routerIpv6Header.setSourceIp(m_routerIp->toIPv6());
}

void OSPF::run()
{
    IpPtr_t fakeDest = IPv4_t::createIpPtr("255.255.255.255", "255.255.255.255");
    QByteArray payload;
    DataLinkHeader *dh = new DataLinkHeader(this->m_routerMac, this->m_routerMac);
    TCPHeader *th = new TCPHeader(BROADCAST_ON_ALL_PORTS, BROADCAST_ON_ALL_PORTS);
    Packet *hello = new Packet(UT::PacketType::Control, UT::PacketControlType::OSPF,
                                        1, 0, 0, fakeDest, payload, *dh, *th, m_routerIpv4Header, m_routerIpv6Header,
                                        OSPF_TTL);

    hello->storeStringInPayload(generateHelloPayload());
    m_lsaPacket = *hello;
    m_lsaIsReady = true;
    /// set hello packet as update packet : DONE!!!
    /// receive others hello -> add the sender ip by the cost 1 to the routing table???
    /// send update packet -> send current ips and costs in the routing table !!!
    /// receive others update packet -> update routing table enties if other update cost + other cost < current cost!!!
    /// if the routing table wasnt updated after n update packets emit end signal
}

void OSPF::handleOSPFPacket(const PacketPtr_t &packet, const QSharedPointer<Port> &port)
{
    QJsonObject update = parsePayload(packet->readStringFromPayload());
    QString type = update["type"].toString();
    if (type == HELLO)
        processHelloPacket(packet, port);
    else
        processLSAPacket(packet, port);
}

void OSPF::processHelloPacket(const PacketPtr_t &packet, const QSharedPointer<Port> &port) {
    qDebug() << "Processing Hello packet from port" << port->number();
    QString payload = packet->readStringFromPayload();
    QJsonObject lsaData = parsePayload(payload);
    QString router = lsaData["router"].toString();
    m_topologyGraph[m_routerIp->toString()].insert(NodeData(router, m_routerIp->toString(), port));//TODO::change to payload ip
    updateLsaPacket();
    // Update neighbor list based on received Hello packet.
    // TODO:: update
}

void OSPF::processLSAPacket(const PacketPtr_t &packet, const QSharedPointer<Port> &port) {
    QString payload = packet->readStringFromPayload();
    QJsonObject lsaData = parsePayload(payload);
    updateTopologyGraph(lsaData, port);
    updateLsaPacket();
}

void OSPF::updateTopologyGraph(const QJsonObject &lsaData, const QSharedPointer<Port> &port) {
    qDebug() << "Updating topology graph.";
    QString router = lsaData["router"].toString();
    QJsonArray links = lsaData["links"].toArray();
    bool isTopologyUpdated = false;

    for (const auto& link : links) {
        QString neighbor = link.toString();
        if (!m_topologyGraph[router].contains(NodeData(neighbor, router, port))){
            m_topologyGraph[router].insert(NodeData(neighbor, router, port));
            if (!m_topologyGraph.contains(neighbor))
                m_topologyGraph[neighbor].insert(NodeData(DUMMY_NODE, neighbor, nullptr));
            isTopologyUpdated = true;
        }
    }
    if (!isTopologyUpdated)
        m_notUpdatedTimes++;
    else
        m_lsaIsReady = true;
    if (m_notUpdatedTimes == OSPF_FINISH_THRESHOLD){
        m_isFinished = true;
        computeRoutingTable();
        Q_EMIT routingFinished(*m_routingTable);
    }
}

void OSPF::updateLsaPacket()
{
    IpPtr_t fakeDest = IPv4_t::createIpPtr("255.255.255.255", "255.255.255.255");
    QByteArray payload;
    DataLinkHeader *dh = new DataLinkHeader(this->m_routerMac, this->m_routerMac);
    TCPHeader *th = new TCPHeader(BROADCAST_ON_ALL_PORTS, BROADCAST_ON_ALL_PORTS);
    Packet *lsa = new Packet(UT::PacketType::Control, UT::PacketControlType::OSPF,
                                        1, 0, 0, fakeDest, payload, *dh, *th, m_routerIpv4Header, m_routerIpv6Header,
                                        OSPF_TTL);

    lsa->storeStringInPayload(generateLSAPayload());
    m_lsaPacket = *lsa;
}
/// topology -> (parent, (child, parent, nexthop port))
/// tempList -> ((child, parent , port), cost)
void OSPF::computeRoutingTable() {
    qDebug() << "Computing routing table using Dijkstra's algorithm.";
    QList<QString> unVisitedNodes = m_topologyGraph.keys();
    unVisitedNodes.removeAll(m_routerIp->toString());
    // QMap<QString, int> visitedNode;
    QMap<NodeData, int> tempList;
    for (NodeData node : m_topologyGraph.value(m_routerIp->toString()))
        tempList.insert(node, NEIGHBOR_COST);//TODO:: nexthop might have bugs

    while(!tempList.isEmpty()){
        NodeData shortestKey = findShortestPath(tempList); // Returns NodeData of best cost node
        int shortestCost = tempList[shortestKey];
        QString node = shortestKey.node;// childIp
        PortPtr_t port = shortestKey.port;
        QString parent = shortestKey.parent;
        tempList.remove(shortestKey);
        IpPtr_t nodeIP;
        IpPtr_t parentIP;
        if (node.contains(':')){
            nodeIP = IPv6_t::createIpPtr(node, DEFAULT_IPV6_PREFIX_LENGTH);
            parentIP = IPv6_t::createIpPtr(parent, DEFAULT_IPV6_PREFIX_LENGTH);
        }
        else{
            nodeIP = IPv4_t::createIpPtr(node, DEFAULT_MASK);
            parentIP = IPv4_t::createIpPtr(parent, DEFAULT_MASK);
        }
        IpPtr_t nextHopIP = m_routingTable->getNextHop(parentIP);
        if (nextHopIP == nullptr)
            nextHopIP = nodeIP;
        m_routingTable->addRoute(nodeIP, nextHopIP, port, OSPF_PROTOCOL, shortestCost);
        unVisitedNodes.removeAll(node);


        ///-------
        for (NodeData node : m_topologyGraph.value(shortestKey.node)){//TODO:: fix updating bug!!
            int newCost = shortestCost + NEIGHBOR_COST;
            NodeData otherPath = pathToSameNode(node.node, tempList);
            if (otherPath.node != DUMMY_NODE){
                if (newCost < tempList[otherPath]){
                    tempList.remove(otherPath);
                    tempList.insert(node, newCost);
                }
            }
            else
                tempList.insert(node, newCost);
        }

    }
    // Implement Dijkstra's algorithm to compute shortest paths.
}


QString OSPF::generateHelloPayload() {
    QJsonObject helloPacket;
    helloPacket["type"] = HELLO;
    helloPacket["router"] = m_routerIp->toString();

    QJsonDocument doc(helloPacket);
    return QString(doc.toJson(QJsonDocument::Compact));
}

QString OSPF::generateLSAPayload() {
    QJsonObject lsaPacket;
    lsaPacket["type"] = LSA;
    lsaPacket["router"] = m_routerIp->toString();

    QJsonArray links;
    for (const auto& neighbor : m_topologyGraph[m_routerIp->toString()]) {
        links.append(neighbor.node);
    }

    lsaPacket["links"] = links;

    QJsonDocument doc(lsaPacket);
    return QString(doc.toJson(QJsonDocument::Compact));
}

QJsonObject OSPF::parsePayload(const QString& payload) {
    QJsonDocument doc = QJsonDocument::fromJson(payload.toUtf8());
    return doc.object();
}

/// tempList -> (parent, ((child, nexthop port), cost))
NodeData OSPF::findShortestPath(QMap<NodeData, int> tempList)
{
    NodeData keyWithSmallestValue;
    int smallestValue = std::numeric_limits<int>::max();

    for (auto it = tempList.constBegin(); it != tempList.constEnd(); ++it) {
        if (it.value() < smallestValue) {
            smallestValue = it.value() ;
            keyWithSmallestValue = it.key();
        }
    }

    return keyWithSmallestValue;


}

NodeData OSPF::pathToSameNode(QString node, QMap<NodeData, int> tempList)
{
    NodeData keyWithSmallestValue;

    for (auto it = tempList.constBegin(); it != tempList.constEnd(); ++it) {
        if (it.key().node == node) {
            keyWithSmallestValue = it.key();
        }
    }

    return keyWithSmallestValue;
}

bool OSPF::isLSAReady() {
    return m_lsaIsReady;
}

// Packet OSPF::getLSAPacket() {
//     m_lsaIsReady = false;
//     // Construct and return the LSA packet.
//     return Packet();
// }
