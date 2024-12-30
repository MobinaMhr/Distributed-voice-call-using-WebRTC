#include "routingtable.h"

RoutingTable::RoutingTable(QObject *parent) :
    QObject {parent}
{}

void RoutingTable::addRoute(const IpPtr_t &destIp, const IpPtr_t &nextHopIp, const QSharedPointer<Port> &port, const QString &protocol, const int metric) {
    m_routingTable[destIp] = {nextHopIp, port, protocol, metric};
}

void RoutingTable::removeRoute(const IpPtr_t &destIp)
{
    m_routingTable.remove(destIp);
}

QSharedPointer<Port> RoutingTable::getPort(const IpPtr_t &destIp) const {
    auto it = m_routingTable.find(destIp);
    if (it != m_routingTable.end()) {
        return it.value().port;
    }
    return nullptr;
}

bool RoutingTable::routeExists(const IpPtr_t &destIp) const
{
    return m_routingTable.contains(destIp);
}

QMap<IpPtr_t, RoutingTable::RouteEntry> RoutingTable::getAllRoutes() const
{
    return m_routingTable;
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
