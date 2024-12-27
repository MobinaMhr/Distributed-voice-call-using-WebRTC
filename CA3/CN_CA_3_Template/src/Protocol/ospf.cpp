#include "ospf.h"

OSPF::OSPF(Router *router, QObject *parent)
    : QObject(parent){}

OSPF::~OSPF(){}

// OSPF::OSPF(Router *router, QObject *parent)
//     : QObject(parent)//,
//     m_router(router),
//     m_helloTimer(new QTimer(this)),
//     m_lsaTimer(new QTimer(this))
// {
//     connect(m_helloTimer, &QTimer::timeout, this, &OSPF::onHelloTimerTimeout);
//     connect(m_lsaTimer, &QTimer::timeout, this, &OSPF::onLsaTimerTimeout);
//     connect(m_router, &Router::sendPacket, this, &OSPF::onPacketReceived);

//     m_helloTimer->setInterval(1000); // Send Hello every 1 second
//     m_lsaTimer->setInterval(5000);  // Send LSAs every 5 seconds
// }

// OSPF::~OSPF()
// {
//     stop();
// }

// void OSPF::start()
// {
//     qDebug() << "OSPF started on Router ID:" << m_router->id();
//     m_helloTimer->start();
//     m_lsaTimer->start();
// }

// void OSPF::stop()
// {
//     m_helloTimer->stop();
//     m_lsaTimer->stop();
//     qDebug() << "OSPF stopped on Router ID:" << m_router->id();
// }

// void OSPF::identifyNeighbors()
// {
//     // Identify neighbors by sending Hello packets
//     sendHelloPacket();
// }

// void OSPF::sendHelloPacket()
// {
//     qDebug() << "Router ID:" << m_router->id() << "sending Hello packets to identify neighbors.";
//     for (const auto &port : m_router->getPorts()) {
//         if (port->state() == UT::PortState::Idle) {
//             // PacketPtr_t helloPacket = QSharedPointer<Packet>::create(/* Fill Hello packet details */);
//             // Q_EMIT m_router->sendPacket(helloPacket, port->number());
//         }
//     }
// }

// void OSPF::sendLsa()
// {
//     qDebug() << "Router ID:" << m_router->id() << "sending LSAs.";
//     // PacketPtr_t lsaPacket = QSharedPointer<Packet>::create(/* Fill LSA details */);
//     // for (const auto &neighbor : m_router->neighbors()) {
//     //     auto port = neighbor->getIdlePort();
//     //     if (port) {
//     //         Q_EMIT m_router->sendPacket(lsaPacket, port->number());
//     //     }
//     // }
// }

// void OSPF::processReceivedLsa(const PacketPtr_t &packet)
// {
//     qDebug() << "Router ID:" << m_router->id() << "processing received LSA.";

//     IpPtr_t sourceIp;

//     if (packet->ipVersion() == UT::IPVersion::IPv4) {
//         auto header = packet->ipv4Header(); // Extract the IPv4 header
//         // sourceIp = QSharedPointer<IPv4_t>::create(header.toString(), DEFAULT_SUBNET_MASK);
//     }
//     else if (packet->ipVersion() == UT::IPVersion::IPv6) {
//         auto header = packet->ipv6Header(); // Extract the IPv6 header
//         // sourceIp = QSharedPointer<IPv6_t>::create(header.toString(), DEFAULT_IPV6_PREFIX_LENGTH);
//     }

//     if (!sourceIp) {
//         qWarning() << "Invalid IP source in received LSA.";
//         return;
//     }

//     // Update the LSDB
//     // QSet<IpPtr_t> links = /* Extract link information from the packet */;
//     // m_lsdb[sourceIp] = links;

//     // Recalculate shortest paths when LSDB changes
//     calculateShortestPaths();
// }

// void OSPF::calculateShortestPaths()
// {
//     qDebug() << "Router ID:" << m_router->id() << "calculating shortest paths.";
//     // Dijkstra dijkstraAlgorithm(m_lsdb, m_router->id());
//     // auto shortestPaths = dijkstraAlgorithm.calculateShortestPaths();

//     // // Update routing table
//     // for (const auto &[destIp, pathInfo] : shortestPaths) {
//     //     IpPtr_t nextHop = pathInfo.nextHop;
//     //     auto port = m_router->getPort(nextHop);
//     //     m_router->addRoutingEntry(destIp, nextHop, port);
//     // }

//     finalizeRouting();
// }

// void OSPF::finalizeRouting()
// {
//     qDebug() << "Router ID:" << m_router->id() << "finalizing routing tables.";
//     m_router->m_routing_table->printRoutingTable();
//     Q_EMIT routingFinalized();
// }

// void OSPF::onHelloTimerTimeout()
// {
//     identifyNeighbors();
// }

// void OSPF::onLsaTimerTimeout()
// {
//     sendLsa();
// }

// void OSPF::onPacketReceived(const PacketPtr_t &packet)
// {
//     if (packet->packetType() == UT::PacketType::Control) {
//         processReceivedLsa(packet);
//     }
// }
