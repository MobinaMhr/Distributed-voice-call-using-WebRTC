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
    // Add a route to the table
    void addRoute(const QSharedPointer<AbstractIP> &destIp, const QSharedPointer<AbstractIP> &nextHopIp, const QSharedPointer<Port> &port);

    // Remove a route from the table
    void removeRoute(const QSharedPointer<AbstractIP> &destIp);

    // Get the port associated with the IP
    QSharedPointer<Port> getPort(const QSharedPointer<AbstractIP> &destIp) const;

    // Check if a route exists for the given IP
    bool routeExists(const QSharedPointer<AbstractIP> &destIp) const;

    // Get all routes in the table
    QList<QPair<QSharedPointer<AbstractIP>, QSharedPointer<Port>>> getAllRoutes() const;

    void printRoutingTable() const;

Q_SIGNALS:

private:
    QMap<QSharedPointer<AbstractIP>, QPair<QSharedPointer<AbstractIP>, QSharedPointer<Port>>> m_routingTable;

    // QMap<QSharedPointer<AbstractIP>, QSharedPointer<Port>> m_routingTable;
    // Mask
    // Subnet Mask
    // Next-hop Gateway
    // Metric
};

#endif    // ROUTINGTABLE_H
