#include "PortBindingManager.h"

PortBindingManager::PortBindingManager(QObject *parent) :
    QObject {parent}
{}

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
