#ifndef RIP_H
#define RIP_H

#include <QTimer>
#include <QSet>
#include <QJsonObject>
#include <QJsonDocument>
#include "../Router/routingtable.h"

class RIP : public QObject {
    Q_OBJECT

public:
    explicit RIP(IPv4Ptr_t routerIp, QObject* parent = nullptr);
    ~RIP() override = default;
    void run();

    void initiateRoutingUpdate();
    void handleRIPPacket(const PacketPtr_t& packet);

private:
    RoutingTable* m_routingTable;
    QSet<IpPtr_t> m_knownDestinations;
    IPv4Ptr_t     m_currentRouterIp;

    void updateRoutingTable(const QJsonObject& routingData, const IpPtr_t& neighborIp, int neighborMetric);

    static QJsonObject createRoutingData(const RoutingTable* routingTable);

Q_SIGNALS:
    void sendRoutingUpdate(PacketPtr_t packet);

private Q_SLOTS:
    void onNeighborTimeout();
};

#endif // RIP_H
