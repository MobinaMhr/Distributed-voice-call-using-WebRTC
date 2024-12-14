#include "Router.h"
#include "qdebug.h"

#include <memory> // for dynamic_cast

const QString LOG_TITLE = "Router:: ";
const int MAX_BUFFER_SIZE = 20;

Router::Router(int id, const MacAddress &macAddress, int portCount, UT::IPVersion ipv, QThread *parent)
    : Node(id, macAddress, portCount, parent),
    m_routing_table(new RoutingTable),
    m_portStates(portCount, UT::PortState::Idle),
    m_ports(portCount),
    m_ipvVersion(ipv) {

    for (int i = 0; i < portCount; ++i) {
        m_ports[i] = QSharedPointer<Port>::create();

        connect(this, &Router::sendPacket, m_ports[i].data(), &Port::sendPacket, Qt::AutoConnection);
        connect(m_ports[i].data(), &Port::packetReceived, this, &Router::receivePacket, Qt::AutoConnection);
    }
}

Router::~Router() {}

void Router::addRoutingEntry(QSharedPointer<AbstractIP> &destinationIp, QSharedPointer<AbstractIP> &nextHopIp, QSharedPointer<Port> &port) {
    m_routing_table->addRoute(destinationIp, nextHopIp, port);
}

bool Router::isBufferAtCapacity() {
    return m_buffer.size() >= MAX_BUFFER_SIZE;
}

int Router::findBufferPositionForPacket(UT::PacketType packetType) {
    if (packetType == UT::PacketType::Data) {
        if (!isBufferAtCapacity()) {
            return static_cast<int>(m_buffer.size());
        }
    } else if (packetType == UT::PacketType::Control) {
        auto it = std::find_if(m_buffer.rbegin(), m_buffer.rend(), [](const PacketPtr_t &pkt) {
            return pkt->packetType() == UT::PacketType::Control;
        });

        if (it != m_buffer.rend()) {
            if (isBufferAtCapacity()) m_buffer.pop_back();
            return static_cast<int>(it.base() - m_buffer.begin());
        } else if (!isBufferAtCapacity()) {
            return -1;
        }
    }

    return -1;
}

void Router::receivePacket(const PacketPtr_t &packet) {
    bufferPacket(packet);
}

void Router::bufferPacket(const PacketPtr_t &packet) {
    int position = findBufferPositionForPacket(packet->packetType());

    if (position == -1) {
        qDebug() << LOG_TITLE << "Packet Dropped: Buffer Full. Sequence:" << packet->sequenceNumber();
        return;
    }

    if (position == static_cast<int>(m_buffer.size())) {
        m_buffer.push_back(std::move(packet));
    } else {
        m_buffer.insert(m_buffer.begin() + position, std::move(packet));
    }

    qDebug() << LOG_TITLE << (packet->packetType() == UT::PacketType::Data ?
                                "Data Packet Added." : "Control Packet Added.")
             << "Buffer Size:" << m_buffer.size();
}

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

            Q_EMIT sendPacket(packetPtr, hostPort->number());
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

QString Router::ipv6Address() const
{
    return m_ipv6Address.toString();
}

QString Router::ipv4Address() const {
    return m_ipv4Address.toString();
}
