#include "Router.h"
#include "qdebug.h"

#include <memory> // for dynamic_cast

const QString LOG_TITLE = "Router:: ";
const int MAX_BUFFER_SIZE = 20;

Router::Router(int id, const MacAddress &macAddress, QThread *parent)
    : Node(id, macAddress, parent), m_routing_table(new RoutingTable) {

    m_portStates.fill(UT::PortState::Idle);
}

Router::~Router() {}

void Router::addRoutingEntry(QSharedPointer<AbstractIP> &destinationIp, QSharedPointer<Port> &nextHop) {
    m_routing_table->addRoute(destinationIp, nextHop);
}

void Router::processPacket(const Packet &packet) {
    if (m_buffer.size() >= static_cast<std::size_t>(MAX_BUFFER_SIZE)) {
        if (packet.packetType() == UT::PacketType::Data) {
            qDebug() << LOG_TITLE << "Packet Dropped: Buffer Full. Sequence:" << packet.sequenceNumber();
            return;
        }
        if (packet.packetType() == UT::PacketType::Control) {
            m_buffer.pop_back();
            qDebug() << LOG_TITLE << "Buffer Full: Removed Oldest Packet.";
        }
    }

    auto newPacket = std::make_unique<Packet>(
      packet.packetType(),      packet.controlType(),       packet.sequenceNumber(),
      packet.waitingCycles(),   packet.totalCycles(),       packet.destinationIP(),
      packet.payload(),         packet.dataLinkHeader(),    packet.tcpHeader(),
      packet.ipv4Header(),      packet.ipv6Header(),        nullptr);

    if (packet.packetType() == UT::PacketType::Data) {
        m_buffer.push_back(std::move(newPacket));
        qDebug() << LOG_TITLE << "Data Packet Added. Buffer Size:" << m_buffer.size();
    }
    else if (packet.packetType() == UT::PacketType::Control) {
        auto it = std::find_if(m_buffer.rbegin(), m_buffer.rend(), [](const std::unique_ptr<Packet> &pkt) {
            return pkt->packetType() == UT::PacketType::Control;
        });

        if (it != m_buffer.rend()) {
            m_buffer.insert(it.base(), std::move(newPacket));
        } else {
            m_buffer.push_front(std::move(newPacket));
        }
        qDebug() << LOG_TITLE << "Control Packet Added. Buffer Size:" << m_buffer.size();
    }
}

/// Find routing port
/// if empty or not
/// if empty send
/// if not go to nex index

void Router::routePackets() {
    for (auto it = m_buffer.begin(); it != m_buffer.end(); /* no increment here */) {
        const auto& packetPtr = *it;

        if (!packetPtr) {
            qDebug() << "Encountered a null packet pointer.";
            it = m_buffer.erase(it);
            continue;
        }

        qDebug() << "Processing packet with sequence number:" << packetPtr->sequenceNumber();

        if (!m_routing_table->routeExists(packetPtr->destinationIP())) {
            qDebug() << "No Route Exists for the packet";
            continue;
        }

        QSharedPointer<Port> hostPort = m_routing_table->getPort(packetPtr->destinationIP());
        int portIndex = hostPort->number();

        if (m_portStates[portIndex] == UT::PortState::Idle) {
            m_portStates[portIndex] = UT::PortState::Busy;
            qDebug() << "Routing packet to port" << portIndex;

/*            // Simulate packet processing
            hostPort->sendPacket(*pa*/cketPtr);
            it = m_buffer.erase(it);
        } else {
            qDebug() << "Port" << portIndex << "is busy. Keeping packet in buffer.";
            ++it;
        }


        bool allPortsBusy = std::all_of(
          m_portStates.begin(),
          m_portStates.end(),
          [](const UT::PortState& state) { return state == UT::PortState::Busy; });

        if (allPortsBusy) {
            qDebug() << "All ports are busy. Stopping packet routing.";
            break;
        }
    }
}

void Router::routePacket(const AbstractIPHeader &header) {
    if (header.ipVersion() == UT::IPVersion::IPv4) {
        const auto *ipHeader = dynamic_cast<const IPHv4_t *>(&header);
        if (!ipHeader) {
            qDebug() << LOG_TITLE << "Invalid Header";
            return;
        }
        //
    }
    else if (header.ipVersion() == UT::IPVersion::IPv6) {
        const auto *ipHeader = dynamic_cast<const IPHv6_t *>(&header);
        if (!ipHeader) {
            qDebug() << LOG_TITLE << "Invalid Header";
            return;
        }
        //
    }
}

void Router::configurePort(int portIndex, const IPv4_t &ipAddress, const MacAddress &macAddress) {
    if (portIndex < 0 || portIndex >= static_cast<int>(m_ports.size())) {
        qDebug() << LOG_TITLE << "Invalid port index:" << portIndex;
        return;
    }

    // PortPtr_t configedPort = PortPtr_t::create();
    // configedPort->setIpAddress(ipAddress.toString());
    // m_ports[portIndex] = configedPort;

    qDebug() << LOG_TITLE << "Configured port" << portIndex << "with IP" << ipAddress.toString() << "and MAC" << macAddress.toString();
}

// switch (packet.ipVersion()) {
//     case UT::IPVersion::IPv4:
//         routePacket(packet.ipv4Header());
//         break;

//     case UT::IPVersion::IPv6:
//         routePacket(packet.ipv6Header());
//         break;

//     default:
//         qDebug() << LOG_TITLE << "Invalid IP Header: Dropping Packet.";
//         break;
// }
