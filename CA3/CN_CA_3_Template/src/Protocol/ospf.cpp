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
        processLSAPacket(packet);
}

void OSPF::processHelloPacket(const PacketPtr_t &packet, const QSharedPointer<Port> &port) {
    qDebug() << "Processing Hello packet from port" << port->number();
    m_topologyGraph[m_routerIp->toString()].insert(packet->ipv4Header().sourceIp());
    updateLsaPacket();
    // Update neighbor list based on received Hello packet.
    // TODO:: update
}

void OSPF::processLSAPacket(const PacketPtr_t &packet) {
    QString payload = packet->readStringFromPayload();
    QJsonObject lsaData = parsePayload(payload);
    updateTopologyGraph(lsaData);
    updateLsaPacket();
}

void OSPF::updateTopologyGraph(const QJsonObject &lsaData) {
    qDebug() << "Updating topology graph.";
    QString router = lsaData["router"].toString();
    QJsonArray links = lsaData["links"].toArray();
    bool isTopologyUpdated = false;

    for (const auto& link : links) {
        QString neighbor = link.toString();
        if (!m_topologyGraph[router].contains(neighbor)){
            m_topologyGraph[router].insert(neighbor);//prevent repeted neighbors
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

void OSPF::computeRoutingTable() {
    qDebug() << "Computing routing table using Dijkstra's algorithm.";
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
        links.append(neighbor);
    }

    lsaPacket["links"] = links;

    QJsonDocument doc(lsaPacket);
    return QString(doc.toJson(QJsonDocument::Compact));
}

QJsonObject OSPF::parsePayload(const QString& payload) {
    QJsonDocument doc = QJsonDocument::fromJson(payload.toUtf8());
    return doc.object();
}

bool OSPF::isLSAReady() {
    return m_lsaIsReady;
}

// Packet OSPF::getLSAPacket() {
//     m_lsaIsReady = false;
//     // Construct and return the LSA packet.
//     return Packet();
// }
