#include "Router.h"
#include "qdebug.h"

#include <memory> // for dynamic_cast

const QString LOG_TITLE = "Router:: ";
const int MAX_BUFFER_SIZE = 20;

Router::Router(int id, const MacAddress &macAddress, QThread *parent)
    : Node(id, macAddress, parent), m_routing_table(new RoutingTable) {}

Router::~Router() {}

QString Router::ipAddress() const {
    QStringList ips;
    // for (const auto &port : m_ports) {
    //     if (!port->ipAddress().toStdString().empty()) {
    //         ips.append(port->ipAddress());
    //     }
    // }
    return ips.join(", ");
}

void Router::addRoutingEntry(QSharedPointer<AbstractIP> &destinationIp, QSharedPointer<Port> &nextHop) {
    m_routing_table->addRoute(destinationIp, nextHop);
}

// void Router::processPacket(const Packet &packet) {
//     qDebug() << LOG_TITLE << "Router";

//     if (m_buffer.size() >= static_cast<std::size_t>(MAX_BUFFER_SIZE)) {
//         if (packet.packetType() == UT::PacketType::Data) {
//             qDebug() << LOG_TITLE << "Packet Dropped due to full buffer.";
//             // Optional: Set a flag or log dropped packet details
//             return;
//         }
//         if (packet.packetType() == UT::PacketType::Control) {
//             m_buffer.pop_back();
//             qDebug() << LOG_TITLE << "Buffer full. Oldest packet removed.";
//         }
//     }

//     // auto newPacket = std::make_unique<Packet>(
//     //   packet.packetType(), packet.controlType(),
//     //   packet.destinationIP(),

//     //   packet.tcpHeader(),
//     //   packet.dataLinkHeader(), packet.payload(),
//     //   packet.sequenceNumber(), packet.waitingCycles(),
//     //   packet.totalCycles(), nullptr);

//     // if (packet.packetType() == UT::PacketType::Data) {
//     //     m_buffer.push_back(std::move(newPacket));
//     //     qDebug() << LOG_TITLE << "Data Packet added to buffer. Current buffer size:" << m_buffer.size();
//     // }

//     // if (packet.packetType() == UT::PacketType::Control) {
//     //     auto it = std::find_if(m_buffer.rbegin(), m_buffer.rend(), [](const std::unique_ptr<Packet> &packetPtr) {
//     //         return packetPtr->packetType() == UT::PacketType::Control;
//     //     });
//     //     if (it != m_buffer.rend()) {
//     //         m_buffer.insert(it.base(), std::move(newPacket));
//     //     } else {
//     //         m_buffer.push_front(std::move(newPacket));
//     //     }
//     //     qDebug() << LOG_TITLE << "Control Packet added to buffer. Current buffer size:" << m_buffer.size();
//     // }

//     // Extracting header information
//     // IPHeader header; // Logic to parse packetInfo into header
//     // routePacket(header);
// }

// void processHeader(AbstractIPHeader *header)
// {
//     if (!header) {
//         qWarning("Invalid header pointer.");
//         return;
//     }

//     switch (header->ipVersion()) {
//         case UT::IPVersion::IPv4: {
//                 auto ipv4Header = dynamic_cast<IPHv4_t *>(header);
//                 if (ipv4Header) {
//                     qDebug() << "Processing IPv4 Header:";
//                     qDebug() << "Source IP:" << ipv4Header->sourceIp();
//                     qDebug() << "Destination IP:" << ipv4Header->destIp();
//                 } else {
//                     qWarning("Failed to cast to IPv4 header.");
//                 }
//                 break;
//             }
//         case UT::IPVersion::IPv6: {
//                 auto ipv6Header = dynamic_cast<IPHv6_t *>(header);
//                 if (ipv6Header) {
//                     qDebug() << "Processing IPv6 Header:";
//                     qDebug() << "Source IP:" << ipv6Header->sourceIp();
//                     qDebug() << "Destination IP:" << ipv6Header->destIp();
//                 } else {
//                     qWarning("Failed to cast to IPv6 header.");
//                 }
//                 break;
//             }
//         default:
//             qWarning("Unknown IP version.");
//             break;
//     }
// }

// void Router::routePacket(const AbstractIPHeader &header) {
//     //
// }

// void Router::printRoutingTable() const {
//     qDebug() << LOG_TITLE << "Routing Table:";
//     // for (auto it = m_routingTable.cbegin(); it != m_routingTable.cend(); ++it) {
//     //     // qDebug() << "Destination:" << it.key() << "Next Hop:" << it.value();
//     // }
// }

// void Router::configurePort(int portIndex, const IPv4_t &ipAddress, const MacAddress &macAddress) {
//     if (portIndex < 0 || portIndex >= static_cast<int>(m_ports.size())) {
//         qDebug() << LOG_TITLE << "Invalid port index:" << portIndex;
//         return;
//     }

//     // PortPtr_t configedPort = PortPtr_t::create();
//     // configedPort->setIpAddress(ipAddress.toString());
//     // m_ports[portIndex] = configedPort;

//     qDebug() << LOG_TITLE << "Configured port" << portIndex << "with IP" << ipAddress.toString() << "and MAC" << macAddress.toString();
// }
