#ifndef ROUTINGTABLE_H
#define ROUTINGTABLE_H

#include "../IP/IP.h"
#include "../Port/Port.h"
#include <QMap>
#include <QSharedPointer>
#include <QString>

class RoutingTable : public QObject
{
    Q_OBJECT

public:
    explicit RoutingTable(QObject *parent = nullptr);
    ~RoutingTable() = default;

public:
    void addRoute(const IpPtr_t &destIp, const IpPtr_t &nextHopIp, const QSharedPointer<Port> &port);
    void removeRoute(const IpPtr_t &destIp);
    QSharedPointer<Port> getPort(const IpPtr_t &destIp) const;
    bool routeExists(const IpPtr_t &destIp) const;
    QList<QPair<IpPtr_t, QSharedPointer<Port>>> getAllRoutes() const;

    void printRoutingTable() const;

Q_SIGNALS:

private:
    QMap<IpPtr_t, QPair<IpPtr_t, QSharedPointer<Port>>> m_routingTable;
    // Mask
    // Subnet Mask
    // Next-hop Gateway
    // Metric
};

#endif    // ROUTINGTABLE_H
