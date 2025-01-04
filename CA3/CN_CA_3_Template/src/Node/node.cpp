#include "Node.h"

Node::Node(int id, const MacAddress &macAddress, int portCount, UT::IPVersion ipVersion, QThread* parent)
    : QThread(parent),
    m_ipVersion(ipVersion),
    m_id(id),
    m_hasIP(false),
    m_macAddress(macAddress),
    m_name("Node_" + QString::number(id)){
    m_state = UT::NodeState::Alive;
}

Node::~Node() {}

// Node* Node::instance(int id, const MacAddress &macAddress, int portCount, QThread* parent) {
//     if (!m_self) {
//         m_self = new Node(id, macAddress, parent);
//     }
//     return m_self;
// }

void Node::release() {
    if (m_self) {
        delete m_self;
        m_self = nullptr;
    }
}

void Node::run() {
    exec();
}

void Node::sendDiscovery()
{
    IpPtr_t fakeDest = IPv4_t::createIpPtr("255.255.255.255", "255.255.255.255");
    QByteArray payload ;
    DataLinkHeader *dh = new DataLinkHeader(this->m_macAddress, this->m_macAddress);
    TCPHeader *th = new TCPHeader(BROADCAST_ON_ALL_PORTS, BROADCAST_ON_ALL_PORTS);
    IPHv4_t *iphv4 = new IPHv4_t();
    IPHv6_t *iphv6 = new IPHv6_t();
    Packet *discovery = new Packet(UT::PacketType::Control, UT::PacketControlType::DHCPDiscovery,
                                             1, 0, 0, fakeDest, payload, *dh, *th, *iphv4, *iphv6,
                                             DHCP_TTL);
    discovery->storeIntInPayload(m_id);
    PacketPtr_t discoveryPt = PacketPtr_t(discovery);
    sendPacket(discoveryPt, BROADCAST_ON_ALL_PORTS);
    //send dhcp discover packet
}

void Node::setIP(QString sugestedIP, QString mask)
{
    m_ipv4Address = IPv4Ptr_t(new IPv4_t(sugestedIP, mask));
    if (m_ipVersion == UT::IPVersion::IPv6)
        m_ipv6Address = m_ipv4Address->toIPv6();
    m_hasIP = true;
}

int Node::id() const {
    return m_id;
}

QString Node::name() const {
    return m_name;
}

MacAddress Node::macAddress() const {
    return m_macAddress;
}

UT::NodeState Node::state() {
    return m_state;
}

void Node::setState(UT::NodeState state) {
    m_state = state;
}

UT::IPVersion Node::ipVersion() const {
    return m_ipVersion;
}

void Node::setIPVersion(UT::IPVersion ipVersion) {
    m_ipVersion = ipVersion;
}

QString Node::ipv6Address() const
{
    return m_ipv6Address->toString();
}

QString Node::ipv4Address() const {
    return m_ipv4Address->toString();
}


