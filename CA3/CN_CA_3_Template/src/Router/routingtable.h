#ifndef ROUTINGTABLE_H
#define ROUTINGTABLE_H

#include "../IP/IP.h"
#include "../Port/Port.h"
#include <QMap>
#include <QSharedPointer>
#include <QString>

/// 4. getNodes, getCosts ( returns QVector of QStringified of IPs , ...)

const QString HELLO = "hello";
const QString DEFAULT_MASK = "255.255.255.0";

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
    RoutingTable(const RoutingTable &other);

    // Assignment Operator
    RoutingTable &operator=(const RoutingTable &other);
    ~RoutingTable() = default;

public:
    void addRoute(const IpPtr_t &destIp, const IpPtr_t &nextHopIp, const QSharedPointer<Port> &port, const QString &protocol, const int metric);
    void removeRoute(const IpPtr_t &destIp);
    QSharedPointer<Port> getPort(const IpPtr_t &destIp) const;
    bool routeExists(const IpPtr_t &destIp) const;
    QMap<IpPtr_t, RouteEntry> getAllRoutes() const;

    void printRoutingTable() const;
    int getRouteCost(const IpPtr_t &destIp) const;
    void updateRoute(const IpPtr_t &destIp, const IpPtr_t &nextHopIp, const QSharedPointer<Port> &port, const int metric);
    QVector<QString> getNodes() const;
    QVector<int> getCosts() const;

Q_SIGNALS:
};

#endif    // ROUTINGTABLE_H
