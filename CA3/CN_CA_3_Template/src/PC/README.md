# PC

This class represents personal computers in a network.
It inherits from the Node class and provides additional functionalities specific to a PC, such as handling packet reception and managing ports, which is a little bit different from a Router(also inheriting from Node class). The class utilizes Qt's signal-slot mechanism for communication.

```cpp
void PC::receivePacket(const PacketPtr_t &packet) {
    bool isMine = false;
    UT::IPVersion packetIpVersion = packet->ipVersion();
    if (packetIpVersion == UT::IPVersion::IPv4){
        if (m_ipv4Address.toString() == packet->ipv4Header().destIp())
            isMine = true;
    }
    else if(packetIpVersion == UT::IPVersion::IPv6){
        if (m_ipv6Address.toString() == packet->ipv6Header().destIp())
            isMine = true;
    }

    if (isMine){
        qDebug() << "PC" << name() << "recieved :";
        packet->print();
    }
    // Process the packet based on specific PC logic
}
```

The `receivePacket()` implemented in the code above receives the **QShared pointer** of a packet.

It first checks if the packet version is IPV4 or IPV6.
Then it compares its header with its own one and finds out it is the packets receiver or not.

if it is the packets receiver, it prints it.

```cpp
PortPtr_t PC::getIdlePort() {
    if (m_port->state() == UT::PortState::Idle) return m_port;
    else return nullptr;
}
```

This methods returns the Idle port of a PC.

An Idle port is a non reserved port which is not bined to another port yet.