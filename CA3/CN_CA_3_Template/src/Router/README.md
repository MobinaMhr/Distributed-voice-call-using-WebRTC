# Router

The `Router` class represents a network router in a simulation or network application. It inherits from the Node class and provides functionalities for managing routing tables, handling packets, and managing ports. The class supports both IPv4 and IPv6 addresses and utilizes Qt's signal-slot mechanism for communication.

```cpp
Router::Router(int id, const MacAddress &macAddress, int portCount, UT::IPVersion ipVersion, QThread *parent)
    : Node(id, macAddress, portCount, ipVersion, parent),
      m_routing_table(new RoutingTable),
      m_ports(portCount) {

    for (int i = 0; i < portCount; ++i) {
        m_ports[i] = QSharedPointer<Port>::create(i);

        connect(this, &Router::sendPacket, m_ports[i].data(), &Port::sendPacket, Qt::AutoConnection);
        connect(m_ports[i].data(), &Port::packetReceived, this, &Router::receivePacket, Qt::AutoConnection);
    }
}
```

Its cunstructor sets some attributes and after that, creates QSharedPonters of its Ports.
These ports get a number.

The portCount is read from config file.

Because a router can send and receive messages via a port, it connects the its sendPacket signal to the sendPacket slot in port class, and vice versa.

```cpp
void Router::addRoutingEntry(IpPtr_t &destinationIp, IpPtr_t &nextHopIp, QSharedPointer<Port> &port) {
    m_routing_table->addRoute(destinationIp, nextHopIp, port);
}
```

Its `addRoutingEntry()` method adds a new entry with data of destination IP address, gateway IP address, and their port.

```cpp
bool Router::isBufferAtCapacity() {
    return m_buffer.size() >= MAX_BUFFER_SIZE;
}
```
This method checks if the buffer has reached to its capacity or not.

```cpp
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
```

This method handles both packet priority and fifo rule in the buffer and returns the position for the new buffer.

Note that this packet could be dropped due to full buffer.
This method is implemented base of the project description.

```cpp
void Router::receivePacket(const PacketPtr_t &packet) {
    bufferPacket(packet);
}
```
This override method buffers the input packet.

```cpp
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
```

This method simply finds the position of the new packet it the queue and if there is place, inserts it, else, dropps the packet.

```cpp
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

        if (m_ports[portIndex]->state() == UT::PortState::Reserved) {
            m_ports[portIndex]->setState(UT::PortState::Sending);
            qDebug() << "Routing packet to port" << portIndex;

            Q_EMIT sendPacket(packetPtr, hostPort->number());
            it = m_buffer.erase(it);
        } else if (m_ports[portIndex]->state() == UT::PortState::Sending) {
            qDebug() << "Port" << portIndex << "is already sending. Keeping packet in buffer.";
            ++it;
        } else {
            qDebug() << "Port" << portIndex << "is not bound and is idle.";
            ++it;
        }

        bool allPortsBusy = std::all_of(
          m_ports.begin(),
          m_ports.end(),
          [](PortPtr_t port) { return (port->state() == UT::PortState::Sending) ||
                                      (port->state() == UT::PortState::Idle); });

        if (allPortsBusy) {
            qDebug() << "All ports are Busy. Stopping packet routing.";
            break;
        }
    }
}
```

This method loops over the buffer and finds all prior packets to send via curresponding port.

The sender port state changes from `reserved` to `Sending`.
None reserved ports or `Idle` ports are not bined to any port to send packets.

It stops looping if all ports are Busy with Sending packets in that cycle.

```cpp
PortPtr_t Router::getIdlePort() {
    for (auto port : m_ports) {
        if (port->state() == UT::PortState::Idle)
            return port;
    }
    return nullptr;
}
```

This returns Idle port to be bined.

```cpp
std::vector<PortPtr_t> Router::getPorts() {
    return m_ports;
}
```
This returns all ports to unbined and change topology bindings.


