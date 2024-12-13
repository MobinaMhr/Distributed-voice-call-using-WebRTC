#include "routingtable.h"

RoutingTable::RoutingTable(QObject *parent) :
    QObject {parent}
{}

void RoutingTable::addRoute(const QSharedPointer<AbstractIP> &destIp, const QSharedPointer<AbstractIP> &nextHopIp, const QSharedPointer<Port> &port) {
    m_routingTable[destIp] = QPair<QSharedPointer<AbstractIP>, QSharedPointer<Port>>(nextHopIp, port);
}

void RoutingTable::removeRoute(const QSharedPointer<AbstractIP> &destIp)
{
    m_routingTable.remove(destIp);
}

QSharedPointer<Port> RoutingTable::getPort(const QSharedPointer<AbstractIP> &destIp) const {
    auto it = m_routingTable.find(destIp);
    if (it != m_routingTable.end()) {
        return it.value().second;
    }
    return nullptr;
}

bool RoutingTable::routeExists(const QSharedPointer<AbstractIP> &destIp) const
{
    return m_routingTable.contains(destIp);
}

QList<QPair<QSharedPointer<AbstractIP>, QSharedPointer<Port>>> RoutingTable::getAllRoutes() const
{
    QList<QPair<QSharedPointer<AbstractIP>, QSharedPointer<Port>>> routes;
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
