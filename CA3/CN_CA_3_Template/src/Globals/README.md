# Globals

The `PacketDetails` struct is designed to encapsulate the details of a network packet, including the sender ID, receiver ID, and packet type. This struct is part of the UT namespace and is declared as a metatype to enable its use in Qt's meta-object system.

Some enums were added as:


The `NodeState` enum is designed to represent the state of a node (either a PC or a Router) in a network simulation or application. It provides a simple way to track whether a node is functioning correctly or has encountered a failure.
```cpp
enum class NodeState
{
    Alive,
    Dead
};
```

The `IPVersion` enum is designed to represent the version of the Internet Protocol (IP) being used by a network node or device. It provides a way to distinguish between IPv4 and IPv6, the two main versions of the IP protocol.
```cpp
enum class IPVersion
{
    IPv4,
    IPv6
};
```

The `PortState` enum is designed to represent the state of a network port in a simulation or network application. The **Reserved** means the port have been binded so far, the **Idle** means it is not binded to any port and the **Sending** a packet in the cycle.
```cpp
enum class PortState
{
    Reserved,
    Idle,
    Sending
};
```