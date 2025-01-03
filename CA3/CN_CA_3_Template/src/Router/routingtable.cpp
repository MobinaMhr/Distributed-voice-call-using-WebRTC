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
    if (!destIp) {
        return false;
    }

    QString destIpString = destIp->toString();

    for (auto it = m_routingTable.cbegin(); it != m_routingTable.cend(); ++it) {
        if (it.key() && it.key()->toString() == destIpString) {
            return true;
        }
    }

    return false;
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

int RoutingTable::getRouteCost(const IpPtr_t &destIp) const {
    if (!destIp) {
        return std::numeric_limits<int>::max();
    }

    auto it = m_routingTable.find(destIp);
    if (it != m_routingTable.end()) {
        return it.value().metric;
    }

    return std::numeric_limits<int>::max();
}

IpPtr_t RoutingTable::getNextHop(const IpPtr_t &destIp) const
{
    auto it = m_routingTable.find(destIp);
    if (it != m_routingTable.end()) {
        return it.value().nextHopIp;
    }
    return nullptr;
}

void RoutingTable::updateRoute(const IpPtr_t &destIp, const IpPtr_t &nextHopIp, const QSharedPointer<Port> &port, const int metric) {
    if (!destIp) {
        qWarning() << "Invalid destination IP.";
        return;
    }

    auto it = m_routingTable.find(destIp);
    if (it != m_routingTable.end()) {
        it.value().nextHopIp = nextHopIp;
        it.value().port = port;
        it.value().metric = metric;
    } else {
        addRoute(destIp, nextHopIp, port, "TODO", metric);
        qWarning() << "A new route for destination IP" << destIp->toString() << "Added.";
    }
}

QVector<QString> RoutingTable::getNodes() const {
    QVector<QString> nodes;
    for (auto it = m_routingTable.constBegin(); it != m_routingTable.constEnd(); ++it) {
        if (it.key()) {
            nodes.append(it.key()->toString());
        }
    }
    return nodes;
}

QVector<int> RoutingTable::getCosts() const {
    QVector<int> costs;
    for (auto it = m_routingTable.constBegin(); it != m_routingTable.constEnd(); ++it) {
        costs.append(it.value().metric);
    }
    return costs;
}

RoutingTable::RoutingTable(const RoutingTable &other) :
    QObject(other.parent())
{
    m_routingTable = other.m_routingTable;
}

RoutingTable &
RoutingTable::operator=(const RoutingTable &other)
{
    if(this != &other)
    {
        QObject::setParent(other.parent());
        m_routingTable = other.m_routingTable;
    }
    return *this;
}
