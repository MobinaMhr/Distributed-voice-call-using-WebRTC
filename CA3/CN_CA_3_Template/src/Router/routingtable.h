#ifndef ROUTINGTABLE_H
#define ROUTINGTABLE_H

#include "../IP/IP.h"
#include "../Port/Port.h"
#include <QMap>
#include <QSharedPointer>
#include <QString>

/// 3. a method to update route that gets destination IP, updated nextHopIp, port, metric and updates them.
/// 4. getNodes, getCosts ( returns QVector of QStringified of IPs , ...)

class RoutingTable : public QObject
{
    Q_OBJECT

public:
    struct RouteEntry {
        IpPtr_t nextHopIp;
        QSharedPointer<Port> port;
        QString protocol;
        int metric;
    };

private:

    QMap<IpPtr_t, RouteEntry> m_routingTable;

public:
    explicit RoutingTable(QObject *parent = nullptr);
    ~RoutingTable() = default;

public:
    void addRoute(const IpPtr_t &destIp, const IpPtr_t &nextHopIp, const QSharedPointer<Port> &port, const QString &protocol, const int metric);
    void removeRoute(const IpPtr_t &destIp);
    QSharedPointer<Port> getPort(const IpPtr_t &destIp) const;
    bool routeExists(const IpPtr_t &destIp) const;
    QMap<IpPtr_t, RouteEntry> getAllRoutes() const;

    void printRoutingTable() const;
    int getRouteCost(const IpPtr_t &destIp) const;

Q_SIGNALS:
};

#endif    // ROUTINGTABLE_H
