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
    void addRoute(const QSharedPointer<AbstractIP> &destIp, const QSharedPointer<AbstractIP> &nextHopIp, const QSharedPointer<Port> &port);
    void removeRoute(const QSharedPointer<AbstractIP> &destIp);
    QSharedPointer<Port> getPort(const QSharedPointer<AbstractIP> &destIp) const;
    bool routeExists(const QSharedPointer<AbstractIP> &destIp) const;
    QList<QPair<QSharedPointer<AbstractIP>, QSharedPointer<Port>>> getAllRoutes() const;

    void printRoutingTable() const;

Q_SIGNALS:

private:
    QMap<QSharedPointer<AbstractIP>, QPair<QSharedPointer<AbstractIP>, QSharedPointer<Port>>> m_routingTable;
    // Mask
    // Subnet Mask
    // Next-hop Gateway
    // Metric
};

#endif    // ROUTINGTABLE_H
