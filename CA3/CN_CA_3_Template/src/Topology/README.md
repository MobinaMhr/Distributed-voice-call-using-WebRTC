# TopologyBuilder

The `TopologyBuilder` class is designed to manage the creation and configuration of network topologies, including PCs and routers. It provides functionalities for creating network nodes, initializing routers, and configuring various network topologies such as **mesh**, **torus**, **star**, and **ring-star**. 

The class also manages the binding and unbinding of ports to simulate network connections.

```cpp
TopologyBuilder::TopologyBuilder(int routerBufferSize, QObject *parent) :
    QObject(parent),
    m_routerBufferSize(routerBufferSize),
    m_portBindingManager(new PortBindingManager(this)),
    m_macAddressGenerator(new MacAddressGenerator(this)) {}
```

This constructor sets some private attributes which are needed in its methods.

```cpp
TopologyBuilder::~TopologyBuilder() {
    delete m_portBindingManager;
    delete m_macAddressGenerator;
    resetBindings(); ///Could be deleted
}
```

It deletes the pointer to mac address generator and port binding manager. It also resets all created port bindings in the previous toploty.

```cpp
void TopologyBuilder::resetBindings() {
    for (auto &router : m_routers) {
        auto ports = router->getPorts();
        for (auto &port : ports) {
            m_portBindingManager.unbind(port);
        }
    }
}
```

This method iterates over all routers and unbinds the connection between its all ports with other routers and pcs.

It uses the pointer of PortBindingManager class to unbind all ports binded to the port.

```cpp
QSharedPointer<PC> TopologyBuilder::createPC(int id, int portCount, UT::IPVersion ipVersion) {
    return QSharedPointer<PC>::create(
      id,
      m_macAddressGenerator->generateMacAddress(),
      portCount,
      ipVersion,
      nullptr
    );
}
```

The private `createPC()` method returns a created QSharedPointer of PC class with its needed input values.

```cpp
QSharedPointer<Router> TopologyBuilder::createRouter(int id, int portCount, UT::IPVersion ipVersion) {
    return QSharedPointer<Router>::create(
      id,
      m_macAddressGenerator->generateMacAddress(),
      portCount,
      ipVersion,
      m_routerBufferSize,
      nullptr
    );
}
```

The private `createRouter()` method returns a created QSharedPointer of Router class with its needed input values.

```cpp
QVector<QSharedPointer<PC>> TopologyBuilder::getPCs(int count, int offset, UT::IPVersion ipVersion, int portCount) {
    QVector<QSharedPointer<PC>> m_pcs;

    for (int id = 0; id < count; ++id) {
        auto pc = createPC(id + offset, portCount, ipVersion);
        m_pcs[id] = pc;
    }

    return m_pcs;
}
```

This method `getPCs()` gets the total PC count in the AS and creates all PCs. Its input offset parameter is used to give different IDs in all Autonomous Systems. 

```cpp
void TopologyBuilder::initializeRouters(int routerCount, UT::IPVersion ipVersion, int offset, int portCount) {
    m_offset = offset;
    m_routerCount = routerCount;
    m_routers.resize(m_routerCount);
    m_rows = routerCount / 2;
    m_columns = routerCount / 2;

    for (int id = 0; id < m_routerCount; ++id) {
        auto router = createRouter(id + 1 + m_offset, portCount, ipVersion);
        m_routers[id] = router;
    }
}
```

The `initializeRouters()` method will create all routers in an AS. Its offset value does the same functionality as describe in `getPCs()` method.

```cpp
void TopologyBuilder::moveToMeshTopology() {
    resetBindings();

    for (int i = 0; i < m_rows; ++i) {
        for (int j = 0; j < m_columns; ++j) {
            int id = i * m_columns + j;
            ///                         0 left      2 right
            if (j > 0) bindPorts(m_routers[id]->getIdlePort(), m_routers[id - 1]->getIdlePort());
            ///                         1 up        3 down
            if (i > 0) bindPorts(m_routers[id]->getIdlePort(), m_routers[id - m_columns]->getIdlePort());
        }
    }
}
```

This method named `moveToMeshTopology()` resets all bindings in all routers if they exists and creates a Mesh topology by binding routers together. 

![Mesh](https://github.com/MobinaMhr/Computer-Networks-Course-S2025/blob/main/CA3/assets/Mesh.png)

```cpp
void TopologyBuilder::moveToTorusTopology() {
    resetBindings();
    moveToMeshTopology();

    for (int i = 0; i < m_rows; ++i) {
        bindPorts(m_routers[i * m_columns]->getIdlePort(), m_routers[i * m_columns + m_columns - 1]->getIdlePort());
    }

    for (int j = 0; j < m_columns; ++j) {
        bindPorts(m_routers[j]->getIdlePort(), m_routers[(m_rows - 1) * m_columns + j]->getIdlePort());
    }
}
```

This method named `moveToTorusTopology()` resets all bindings in all routers if they exists and creates a Torus topology by binding routers together.

![Torus](https://github.com/MobinaMhr/Computer-Networks-Course-S2025/blob/main/CA3/assets/Torus.png)

```cpp
void TopologyBuilder::moveToStarTopology() {
    resetBindings();

    auto centerNode = m_routers[0];
    for (int i = 1; i <= m_routerCount; ++i) {
        auto node = m_routers[i];
        bindPorts(centerNode->getIdlePort(), node->getIdlePort());
    }
}
```

This method named `moveToStarTopology()` resets all bindings in all routers if they exists and creates a Star topology by binding routers together.

```cpp
void TopologyBuilder::moveToRingStarTopology() {
    if (m_routerCount < 3) return;

    resetBindings();
    auto centerNode = m_routers.back();

    int i = 0;
    for (i = 0; i < m_routerCount - 1; ++i) {
        auto node = m_routers[i];
        if (i % 2 == 0) bindPorts(centerNode->getIdlePort(), node->getIdlePort());
        if (i > 0)      bindPorts(m_routers[i]->getIdlePort(), m_routers[i - 1]->getIdlePort());
    }
    bindPorts(m_routers[i-1]->getIdlePort(), m_routers[0]->getIdlePort());
}
```

This method named `moveToRingStarTopology()` resets all bindings in all routers if they exists and creates a Ring Star topology by binding routers together.

![RingStar](https://github.com/MobinaMhr/Computer-Networks-Course-S2025/blob/main/CA3/assets/RingStar.png)

```cpp
bool TopologyBuilder::bindPorts(PortPtr_t port1, PortPtr_t port2) {
    if (port1 == nullptr || port1->state() != UT::PortState::Idle) {
        qDebug() << "Give port1 is not idle, cannot be bined.";
        return false;
    }
    if (port2 == nullptr || port2->state() != UT::PortState::Idle) {
        qDebug() << "Give port2 is not idle, cannot be bined.";
        return false;
    }

    m_portBindingManager->bind(port1, port2);
    return true;
}
```

This method check if the input ports are valid to bind to each other or not and then, uses the `m_portBindingManager` to bind them.

```cpp
bool TopologyBuilder::unbindPorts(PortPtr_t port1, PortPtr_t port2) {
    if (port1 == nullptr || port1->state() == UT::PortState::Idle) {
        qDebug() << "Give port1 is idle, cannot be unbined.";
        return false;
    }
    if (port2 == nullptr || port2->state() == UT::PortState::Idle) {
        qDebug() << "Give port2 is idle, cannot be unbined.";
        return false;
    }

    m_portBindingManager->bind(port1, port2);
    return true;
}
```

This method check if the input ports are valid to unbind from each other or not and then, uses the `m_portBindingManager` to unbind them.

```cpp
QVector<QSharedPointer<Router>> TopologyBuilder::routers() {
    return m_routers;
}
```

This methor returns QShared Ponters of all built routers in the topology/ 

# TopologyController


