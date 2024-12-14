#include "routingtable.h"

RoutingTable::RoutingTable(QObject *parent) :
    QObject {parent}
{}

void RoutingTable::addRoute(const IpPtr_t &destIp, const IpPtr_t &nextHopIp, const QSharedPointer<Port> &port) {
    m_routingTable[destIp] = QPair<IpPtr_t, QSharedPointer<Port>>(nextHopIp, port);
}

void RoutingTable::removeRoute(const IpPtr_t &destIp)
{
    m_routingTable.remove(destIp);
}

QSharedPointer<Port> RoutingTable::getPort(const IpPtr_t &destIp) const {
    auto it = m_routingTable.find(destIp);
    if (it != m_routingTable.end()) {
        return it.value().second;
    }
    return nullptr;
}

bool RoutingTable::routeExists(const IpPtr_t &destIp) const
{
    return m_routingTable.contains(destIp);
}

QList<QPair<IpPtr_t, QSharedPointer<Port>>> RoutingTable::getAllRoutes() const
{
    QList<QPair<IpPtr_t, QSharedPointer<Port>>> routes;
    for (auto it = m_routingTable.constBegin(); it != m_routingTable.constEnd(); ++it)
    {
        routes.append(qMakePair(it.key(), it.value().second));
    }
    return routes;
}

void RoutingTable::printRoutingTable() const
{
    if (m_routingTable.isEmpty()) {
        qDebug() << "Routing Table is empty.";
        return;
    }

    qDebug() << "Routing Table:";
    for (auto it = m_routingTable.constBegin(); it != m_routingTable.constEnd(); ++it) {
    }
}
