# Port

The Port class is designed to represent a network port in a simulation or network application. It inherits from QObject and provides functionalities for sending and receiving packets, managing port states, which is defined in `Globals.h`, and handling IP addresses. The class supports both IPv4 and IPv6 addresses.

Its constructor simply sets the values defined above.

```cpp
Port::Port(const Port &other)
    : QObject(other.parent()),
    m_number(other.m_number),
    m_numberOfPacketsSent(other.m_numberOfPacketsSent),
    m_ipVersion(other.m_ipVersion)
{}
```

<!-- It has an Assignment Operator Overloading coded as:
```cpp
Port& Port::operator=(const Port &other)
{
    if (this != &other) {
        m_number = other.m_number;
        m_numberOfPacketsSent = other.m_numberOfPacketsSent;
        m_ipVersion = other.m_ipVersion;
        QObject::setParent(other.parent());
    }
    return *this;
}
```

It helps coping the values from another port object to the current opject.

We feeled  -->

Its destructor does nothing.

```cpp
void Port::sendPacket(const PacketPtr_t &data, int portNumber)
{
    if (portNumber == m_number || portNumber == BROADCAST_ON_ALL_PORTS){
        ++m_numberOfPacketsSent;
        Q_EMIT packetSent(data);
    }
}
```
### **public Q_SLOTS**:

Its slots contin these methods:

- sendPacket:
```cpp
void Port::sendPacket(const PacketPtr_t &data, int portNumber)
{
    if (portNumber == m_number || portNumber == BROADCAST_ON_ALL_PORTS){
        ++m_numberOfPacketsSent;
        Q_EMIT packetSent(data);
    }
}
```

-----------------------------------------TODO
<!-- if it is trying to Broadcast to all ports or simply wants to send a packet to a specific  -->

- receivePacket
```cpp
void Port::receivePacket(const PacketPtr_t &data)
{
    Q_EMIT packetReceived(data);
}
```

It also has implemented some getters and setters.
```cpp
uint8_t Port::number()
{
    return m_number;
}

QString Port::ipAddress()
{
    return (m_ipVersion == UT::IPVersion::IPv4) ?
             ipv4Address() :
             ipv6Address();
}

QString Port::ipv6Address() const
{
    return m_ipv6Address.toString();
}

QString Port::ipv4Address() const {
    return m_ipv4Address.toString();
}

UT::PortState Port::state() {
    return m_state;
}

void Port::setState(UT::PortState state) {
    m_state = state;
}
```