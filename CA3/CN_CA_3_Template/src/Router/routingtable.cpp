#include "routingtable.h"

RoutingTable::RoutingTable(QObject *parent) :
    QObject {parent}
{}

void RoutingTable::addRoute(const QSharedPointer<AbstractIP> &ip, const QSharedPointer<Port> &port)
{
    m_routingTable[ip] = port;
}

void RoutingTable::removeRoute(const QSharedPointer<AbstractIP> &ip)
{
    m_routingTable.remove(ip);
}

QSharedPointer<Port> RoutingTable::getPort(const QSharedPointer<AbstractIP> &ip) const
{
    return m_routingTable.value(ip, nullptr);
}

bool RoutingTable::routeExists(const QSharedPointer<AbstractIP> &ip) const
{
    return m_routingTable.contains(ip);
}

QList<QPair<QSharedPointer<AbstractIP>, QSharedPointer<Port>>> RoutingTable::getAllRoutes() const
{
    QList<QPair<QSharedPointer<AbstractIP>, QSharedPointer<Port>>> routes;
    for (auto it = m_routingTable.constBegin(); it != m_routingTable.constEnd(); ++it)
    {
        routes.append(qMakePair(it.key(), it.value()));
    }
    return routes;
}
