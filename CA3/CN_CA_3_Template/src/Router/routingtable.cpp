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

void RoutingTable::printRoutingTable() const
{
    if (m_routingTable.isEmpty()) {
        qDebug() << "Routing Table is empty.";
        return;
    }

    qDebug() << "Routing Table:";
    for (auto it = m_routingTable.constBegin(); it != m_routingTable.constEnd(); ++it) {
        // Print the IP and associated port
        // qDebug() << "Destination IP:" << it.key()->toString()
        //          << ", Next Hop (Port):" << it.value()->toString();
    }
}
