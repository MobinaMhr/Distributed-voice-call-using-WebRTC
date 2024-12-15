# PortBindintManager

The `PortBindingManager` class is designed to manage the binding and unbinding of network ports. It inherits from QObject and provides functionalities for establishing and removing connections between ports, simulating **full-duplex** communication, and managing port states.

```cpp
void PortBindingManager::bind(const PortPtr_t &port1, const PortPtr_t &port2)
{
    // Bind the ports: simulate full-duplex communication.
    if (!bindings.contains(port1))
    {
        bindings[port1] = QList<PortPtr_t>();
    }

    if (!bindings.contains(port2))
    {
        bindings[port2] = QList<PortPtr_t>();
    }

    if (!bindings[port1].contains(port2)){
        //TODO : handle possible errors in next phase !!
        bindings[port1].append(port2);
        bindings[port2].append(port1);

        port1->setState(UT::PortState::Reserved);
        port2->setState(UT::PortState::Reserved);
    }

    Q_EMIT bindingChanged(port1->ipAddress(), port1->number(), port2->ipAddress(), port2->number(), true);

    connect(port1.get(), &Port::packetSent, port2.get(), &Port::receivePacket, Qt::AutoConnection);
    connect(port2.get(), &Port::packetSent, port1.get(), &Port::receivePacket, Qt::AutoConnection);
}
```

The `bind()` method bindes two input ports given as QShared Pointers together, so it simulates a full duplex communication as it was wanted.

Because of full duplex nature, for all pairs of two ports this method is just called once.

As this class has a list structure for all bined ports for a specific port, it fist checks if there was not created such a list for input ports, it creates the lists separately.

it checks if they were not bined before, binds them and changes the port states to the `Reserved`.

Then emits the signal and the connection of `packetSent` between two ports are stablished.

```cpp
bool PortBindingManager::unbind(const PortPtr_t &port1, const PortPtr_t &port2)
{
    if (bindings.contains(port1) && bindings[port1].contains(port2))
    {
        bindings[port1].removeOne(port2);
        bindings[port2].removeOne(port1);
        port1->setState(UT::PortState::Idle);
        port2->setState(UT::PortState::Idle);

        disconnect(port1.get(), &Port::packetSent, port2.get(), &Port::receivePacket);
        disconnect(port2.get(), &Port::packetSent, port1.get(), &Port::receivePacket);

        Q_EMIT bindingChanged(port1->ipAddress(), port1->number(), port2->ipAddress(), port2->number(), false);
        return true;
    }
    return false;
}
```

The `unbind()` method unbinds a port from all other ports it is currently bound to. Then emitts the bindingChanged signal and returns true, if it was not possible to unbind these two ports, it returns false.

```cpp
bool PortBindingManager::unbind(const PortPtr_t &port1)
{
    if (bindings.contains(port1))
    {
        for (auto port2 : bindings[port1]) {
            bindings[port1].removeOne(port2);
            bindings[port2].removeOne(port1);

            port2->setState(UT::PortState::Idle);

            disconnect(port1.get(), &Port::packetSent, port2.get(), &Port::receivePacket);
            disconnect(port2.get(), &Port::packetSent, port1.get(), &Port::receivePacket);

            Q_EMIT bindingChanged(port1->ipAddress(), port1->number(), port2->ipAddress(), port2->number(), false);
        }
        port1->setState(UT::PortState::Idle);
        return true;
    }
    return false;
}
```

This method gets just one port and does the unbinding between all binded ports to this input port.
The process is similar to previous method.

