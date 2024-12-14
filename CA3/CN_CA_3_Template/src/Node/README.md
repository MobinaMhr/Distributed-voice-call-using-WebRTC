# Node

The `Node` class represents a network node in a simulation or network application. 

It inherits from `QThread`. The use of QThread allows the node to run in its own thread, enabling **concurrent** processing.

It provides functionalities for managing node attributes such as `ID`, `MAC address`, `IP addresses`, supporting both IPV4 and IPV6 versions, and node `state`, to simulate failure in nodes. 

```cpp
Node::Node(int id, const MacAddress &macAddress, int portCount, UT::IPVersion ipVersion, QThread* parent)
    : QThread(parent),
    m_id(id),
    m_macAddress(macAddress),
    m_name("Node_" + QString::number(id)),
    m_ipVersion(ipVersion) {
    m_state = UT::NodeState::Alive;
}
```
Its constructor gets and sets some values to its attributes.

Its destructor does nothing.

```cpp
void Node::release() {
    if (m_self) {
        delete m_self;
        m_self = nullptr;
    }
}
```
Its release method deletes the single instance of the Node class if it exists and sets the pointer to nullptr.
It is used for the thread functionality.

The `run()` method starts the event loop for the thread.
```cpp
void Node::run() {
    exec();
}
```

And also it has some getters and setters.
```cpp
int Node::id() const {
    return m_id;
}
QString Node::name() const {
    return m_name;
}
QString Node::macAddress() const {
    return m_macAddress.toString();
}
UT::NodeState Node::state() {
    return m_state;
}
void Node::setState(UT::NodeState state) {
    m_state = state;
}
QString Node::ipv6Address() const {
    return m_ipv6Address.toString();
}
QString Node::ipv4Address() const {
    return m_ipv4Address.toString();
}

```