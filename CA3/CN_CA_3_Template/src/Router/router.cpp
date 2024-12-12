#include "Router.h"
#include "qdebug.h"

const QString LOG_TITLE = "Router:: ";

Router::Router(int id, const MacAddress &macAddress, QThread *parent)
    : Node(id, macAddress, parent) {}

Router::~Router() {}

QString Router::ipAddress() const {
    QStringList ips;
    for (const auto &port : m_ports) {
        if (!port->ipAddress().toStdString().empty()) {
            ips.append(port->ipAddress());
        }
    }
    return ips.join(", ");
}

void Router::addRoutingEntry(const QString &destination, const QString &nextHop) {
    m_routingTable.insert(destination, nextHop);
    qDebug() << LOG_TITLE << "Routing entry added: Destination" << destination << "Next hop" << nextHop;
}

void Router::processPacket(const PacketDetails &packetDetails) {
    qDebug() << LOG_TITLE << "Router" << name();
    qDebug() << LOG_TITLE << "Received packet type:" << static_cast<int>(packetDetails.packetType);
    qDebug() << LOG_TITLE << "Receiver ID:" << packetDetails.receiverID;
    qDebug() << LOG_TITLE << "Sender ID:" << packetDetails.senderID;

    if (packetDetails.packetType == PacketType::Data) {
        if (m_buffer.size() >= MAX_BUFFER_SIZE) {
            qDebug() << LOG_TITLE << "Packet Dropped due to full buffer.";
            //TODO:: True the droped flag.
        } else {
            m_buffer.push_back(packetDetails);
            qDebug() << LOG_TITLE << "Data Packet added to buffer. Current buffer size:" << m_buffer.size();
        }
    }

    if (packetDetails.packetType == PacketType::Control) {
        if (m_buffer.size() == MAX_BUFFER_SIZE) {
            //TODO:: True the droped flag.
            m_buffer.pop_back();
        }

        auto it = std::find_if(m_buffer.rbegin(), m_buffer.rend(), [](const PacketDetails &packet) {
            return packet.packetType == PacketType::Control;
        });

        if (it != m_buffer.rend()) {
            m_buffer.insert(it.base(), packetDetails);
        } else {
            m_buffer.push_front(packetDetails);
        }
        qDebug() << LOG_TITLE << "Control Packet added to buffer. Current buffer size:" << m_buffer.size();
    }

    // Extracting header information
    // IPHeader header; // Logic to parse packetInfo into header
    // routePacket(header);
}

// void Router::routePacket(const IPHeader &header) {
//     QString destination = header.destinationIP();
//     if (m_routingTable.contains(destination)) {
//         QString nextHop = m_routingTable[destination];
//         qDebug() << LOG_TITLE << "Routing packet destined for" << destination << "to next hop" << nextHop;
//         // Logic to forward the packet to the next hop
//     } else {
//         qDebug() << LOG_TITLE << "No routing entry for destination" << destination << ". Dropping packet.";
//     }
// }

void Router::printRoutingTable() const {
    qDebug() << LOG_TITLE << "Routing Table:";
    for (auto it = m_routingTable.cbegin(); it != m_routingTable.cend(); ++it) {
        qDebug() << "Destination:" << it.key() << "Next Hop:" << it.value();
    }
}

void Router::configurePort(int portIndex, const IPv4_t &ipAddress, const MacAddress &macAddress) {
    if (portIndex < 0 || portIndex >= static_cast<int>(m_ports.size())) {
        qDebug() << LOG_TITLE << "Invalid port index:" << portIndex;
        return;
    }

    PortPtr_t configedPort = PortPtr_t::create();
    configedPort->setIpAddress(ipAddress.toString());
    configedPort->setMacAddress(macAddress);
    m_ports[portIndex] = configedPort;

    qDebug() << LOG_TITLE << "Configured port" << portIndex << "with IP" << ipAddress.toString() << "and MAC" << macAddress.toString();
}
