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


This method initializes the DHCP (Dynamic Host Configuration Protocol) for the router. It takes an integer parameter asNumber which is used to create a new instance of the DHCP class. The newly created DHCP object is assigned to the member variable m_dhcp.
```cpp
void Router::setDhcp(int asNumber)
{
    this->m_dhcp = new DHCP(asNumber);
}
```

This method sets the routing protocol for the router. It takes a parameter protocol of type UT::PacketControlType. The method sets the member variable m_isRouting to true and assigns the protocol to m_protocol. If the protocol is RIP (Routing Information Protocol), it creates a new RIP object with the router's IPv4 address and MAC address, connects the routingFinished signal of the RIP object to the finishRouting slot of the Router class, and starts the RIP routing process by calling the run method of the RIP object.
```cpp
void Router::route(UT::PacketControlType protocol)
{
    m_isRouting = true;
    m_protocol = protocol;
    if (m_protocol == UT::PacketControlType::RIP){
        m_rip = *(new RIP(m_ipv4Address, m_macAddress, this));
        connect(&m_rip, &RIP::routingFinished, this, &Router::finishRouting, Qt::AutoConnection);
        m_rip.run();
    }
}
```

This method returns a vector of shared pointers to Node objects representing the router's neighbors. The method simply returns the member variable m_neighbors.
```cpp
std::vector<QSharedPointer<Node>> Router::neighbors() {
    return m_neighbors;
}
```

This method processes control packets received by the router. It takes two parameters: a shared pointer to a Packet object (packet) and a port number (portNumber). The method uses a switch statement to handle different types of control packets based on the controlType of the packet. It calls specific handler methods for each type of control packet, such as handleDhcpDiscovery, handleDhcpOffer, handleDhcpReq, handleDhcpAck, and m_rip.handleRIPPacket. The qDebug statement is used for debugging purposes to print the router's name and a message indicating that control packet handling logic should be implemented.
```cpp
void Router::processControlPacket(const PacketPtr_t &packet, uint8_t portNumber) {
    qDebug() << name() << ": Implement control packet handling logic.";
    switch (packet->controlType()) {
        case UT::PacketControlType::DHCPDiscovery:
            handleDhcpDiscovery(packet);
            break;

        case UT::PacketControlType::DHCPOffer:
            handleDhcpOffer(packet);
            break;

        case UT::PacketControlType::DHCPRequest:
            handleDhcpReq(packet);
            break;

        case UT::PacketControlType::DHCPAcknowledge:
            handleDhcpAck(packet);
            break;
        case UT::PacketControlType::RIP:
            m_rip.handleRIPPacket(packet, m_ports[portNumber]);
            break;

        default:
            break;
    }
}
```

This method handles DHCP discovery packets. If the m_dhcp member is not nullptr, it reads an ID from the packet payload and assigns an IP address to the node using the m_dhcp object. It then creates a new DHCP offer packet with the assigned IP address and buffers it for sending. If m_dhcp is nullptr, it simply emits the received packet to all ports.
```cpp
void Router::handleDhcpDiscovery(PacketPtr_t packet)
{
    if (m_dhcp != nullptr){
        int id = packet->readIntFromPayload();
        QString sugestedIp = m_dhcp->assignIPToNode(id);
        IpPtr_t fakeDest = IPv4_t::createIpPtr("255.255.255.255", "255.255.255.255");
        QByteArray payload ;
        DataLinkHeader *dh = new DataLinkHeader(this->m_macAddress, packet->dataLinkHeader().destination());

        TCPHeader *th = new TCPHeader(BROADCAST_ON_ALL_PORTS, BROADCAST_ON_ALL_PORTS);
        IPHv4_t *iphv4 = new IPHv4_t();
        IPHv6_t *iphv6 = new IPHv6_t();
        Packet *offer = new Packet(UT::PacketType::Control, UT::PacketControlType::DHCPOffer,
                                           1, 0, 0, fakeDest, payload, *dh, *th, *iphv4, *iphv6,
                                           DHCP_TTL);
        offer->storeStringInPayload(sugestedIp);
        PacketPtr_t offerPt = PacketPtr_t(offer);
        /// BUFFER HERE instead of send

        // Q_EMIT sendPacket(offerPt, BROADCAST_ON_ALL_PORTS);
        // generate offer packet
    }
    else
        Q_EMIT sendPacket(packet, BROADCAST_ON_ALL_PORTS);
}
```

This method handles DHCP offer packets. If the source MAC address of the packet matches the router's MAC address and the router's IPv4 address is the default value, it reads the suggested IP address from the packet payload and creates a new DHCP request packet with the router's ID. The request packet is then emitted to all ports. If the source MAC address does not match, the received packet is emitted to all ports.
```cpp
void Router::handleDhcpOffer(PacketPtr_t packet)
{
    if (packet->dataLinkHeader().source().toString() == m_macAddress.toString()){
        if (m_ipv4Address->toValue() == DEFAULT_IP_VALUE){
            QString sugestedIp = packet->readStringFromPayload();
            IpPtr_t fakeDest = IPv4_t::createIpPtr("255.255.255.255", "255.255.255.255");
            QByteArray payload ;
            DataLinkHeader *dh = new DataLinkHeader(this->m_macAddress,
                                                       packet->dataLinkHeader().destination());
            TCPHeader *th = new TCPHeader(BROADCAST_ON_ALL_PORTS, BROADCAST_ON_ALL_PORTS);
            IPHv4_t *iphv4 = new IPHv4_t();
            IPHv6_t *iphv6 = new IPHv6_t();
            Packet *req = new Packet(UT::PacketType::Control, UT::PacketControlType::DHCPRequest, 1, 0, 0, 
                                     fakeDest, payload, *dh, *th, *iphv4, *iphv6, DHCP_TTL);
            req->storeIntInPayload(m_id);
            PacketPtr_t reqPt = PacketPtr_t(req);
            Q_EMIT sendPacket(reqPt, BROADCAST_ON_ALL_PORTS);
        }
    }
    else
        Q_EMIT sendPacket(packet, BROADCAST_ON_ALL_PORTS);
}
```

This method handles DHCP request packets. If the m_dhcp member is not nullptr, it creates a DHCP acknowledgment packet with the response generated by the createDhcpAckBody method. The acknowledgment packet is then emitted to all ports. If m_dhcp is nullptr, the received packet is emitted to all ports.
```cpp
void Router::handleDhcpReq(PacketPtr_t packet)
{
    if (m_dhcp != nullptr){
        QString response = createDhcpAckBody(packet);
        IpPtr_t fakeDest = IPv4_t::createIpPtr("255.255.255.255", "255.255.255.255");
        QByteArray payload ;
        DataLinkHeader *dh = new DataLinkHeader(this->m_macAddress,
                                                   packet->dataLinkHeader().destination());
        TCPHeader *th = new TCPHeader(BROADCAST_ON_ALL_PORTS, BROADCAST_ON_ALL_PORTS);
        IPHv4_t *iphv4 = new IPHv4_t();
        IPHv6_t *iphv6 = new IPHv6_t();
        Packet *ack = new Packet(UT::PacketType::Control, UT::PacketControlType::DHCPAcknowledge,
                                           1, 0, 0, fakeDest, payload, *dh, *th, *iphv4, *iphv6,
                                           DHCP_TTL);
        ack->storeStringInPayload(response);
        PacketPtr_t ackPt = PacketPtr_t(ack);
        Q_EMIT sendPacket(ackPt, BROADCAST_ON_ALL_PORTS);
        // generate offer packet
    }
    else
        Q_EMIT sendPacket(packet, BROADCAST_ON_ALL_PORTS);
}
```

This method handles DHCP acknowledgment packets. It reads the payload from the packet, parses it as a JSON document, and extracts the IP address and mask. The extracted IP address and mask are then set for the router using the setIP method.
```cpp
void Router::handleDhcpAck(PacketPtr_t packet)
{
    QString payload = packet->readStringFromPayload();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(payload.toUtf8());
    QString ip = jsonDoc.object().value("ip").toString();
    QString mask = jsonDoc.object().value("mask").toString();
    setIP(ip, mask);
}
```

This method handles the reception of packets. If the packet is nullptr, it logs a debug message and returns. It decreases the packet's TTL and checks if the packet is destined for the router or should be dropped. If not, the packet is buffered. The method then processes the packet based on its type (control or data) by calling the appropriate handler methods.
```cpp
void Router::receivePacket(const PacketPtr_t &packet, uint8_t portNumber) {
    if (!packet) {
        qDebug() << name() << ": Received a null packet.";
        return;
    }
    packet->decreasePacketTtl();

    if (!isPacketMine(packet) && !packet->shouldDrop()) {
        bufferPacket(packet);
        return;
    }//TODO:should move somewhere else;

    switch (packet->packetType()) {
        case UT::PacketType::Control:
            processControlPacket(packet, portNumber);
            break;

        case UT::PacketType::Data:
            processDataPacket(packet);
            break;

        default:
            break;
    }
}
```

This method is called when the routing process is finished. It updates the router's routing table with the provided routingTable and sets the m_isRouting member to false. It then emits the routingFinished signal with the router's ID.
```cpp
void Router::finishRouting(RoutingTable routingTable)
{
    m_routing_table = new RoutingTable(routingTable);
    m_isRouting = false;
    Q_EMIT routingFinished(m_id);
}
```

This method retrieves an IP address for the router. If the m_dhcp member is not nullptr, it assigns an IP address to the router using the m_dhcp object and sets the IP address with the default mask. If m_dhcp is nullptr, it sends a DHCP discovery packet.
```cpp
void Router::getIP()
{
    if (m_dhcp != nullptr){
        QString sugestedIp = m_dhcp->assignIPToNode(m_id);
        setIP(sugestedIp, DEFAULT_MASK);
    }else
        sendDiscovery();
}
```

This method handles routing packets if the router is currently routing. It calls the handleRoutingPacket method to process routing packets.
```cpp
void Router::handleTick(int cycleNumebt)
{
    if (m_isRouting){
        handleRoutingPacket();
    }
}
```

This method creates the body of a DHCP acknowledgment packet. It reads an ID from the packet payload and assigns an IP address to the node using the m_dhcp object. It then creates a JSON document with the assigned IP address and default mask and returns the JSON string.
```cpp
QString Router::createDhcpAckBody(PacketPtr_t packet)
{
    int id = packet->readIntFromPayload();
    QString sugestedIp = m_dhcp->assignIPToNode(id);
    QJsonObject jsonObject;
    jsonObject["ip"] = sugestedIp;
    jsonObject["mask"] = DEFAULT_MASK;

    QJsonDocument jsonDoc(jsonObject);

    QString jsonString = jsonDoc.toJson(QJsonDocument::Compact);

    return jsonString;
}
```

This method handles routing packets for the RIP protocol. If the m_protocol member is set to RIP and the m_rip object has an update ready, it creates a new packet with the update and emits it to all ports.
```cpp
void Router::handleRoutingPacket()
{
    if (m_protocol == UT::PacketControlType::RIP){
        if (m_rip.isUpdateReady()){
            Packet *update = new Packet(m_rip.getUpdatePacket());
            Q_EMIT sendPacket(PacketPtr_t(update), BROADCAST_ON_ALL_PORTS);
        }
    }
}
```