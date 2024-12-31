#include "RIP.h"
#include <QDebug>

RIP::RIP(IPv4Ptr_t routerIp, QObject *parent):
    m_currentRouterIp(routerIp) {
    m_routingTable = new RoutingTable(this);
}

void RIP::run()
{
    /// send hello packet
    /// receive others hello -> add the sender ip by the cost 1 to the routing table???
    /// send update packet -> send current ips and costs in the routing table !!!
    /// receive others update packet -> update routing table enties if other update cost + other cost < current cost!!!
    /// if the routing table wasnt updated after n update packets emit end signal
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
