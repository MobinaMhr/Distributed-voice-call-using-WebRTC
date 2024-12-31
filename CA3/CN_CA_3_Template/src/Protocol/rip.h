#ifndef RIP_H
#define RIP_H

#include <QTimer>
#include <QSet>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QVector>
#include "../Router/routingtable.h"

const int RIP_TTL = 16;

class RIP : public QObject {
    Q_OBJECT

public:
    explicit RIP(IPv4Ptr_t routerIp, QObject* parent = nullptr);
    ~RIP() override = default;
    void run();

    void initiateRoutingUpdate();
    void handleRIPPacket(const PacketPtr_t& packet, int portNumebr);

private:
    RoutingTable* m_routingTable;
    QSet<IpPtr_t> m_knownDestinations;
    IPv4Ptr_t     m_currentRouterIp;
    Packet m_updatePacket;
    MacAddress m_routerMacAddress;
    IPHv4_t m_routerIpv4Header;
    IPHv6_t m_routerIpv6Header;

    QString generateUpdatePayload(QString type, QVector<IpPtr_t> nodes, QVector<int> costs);
    QJsonObject extractUpdatePayloadJson(const QString& jsonString);
    QVector<QString> extractNodes(QJsonObject update);
    QVector<int> extractCosts(QJsonObject update);

    void updateRoutingTable(const QJsonObject& routingData, const IpPtr_t& neighborIp, int neighborMetric);

    static QJsonObject createRoutingData(const RoutingTable* routingTable);

Q_SIGNALS:
    void sendRoutingUpdate(PacketPtr_t packet);

private Q_SLOTS:
    void onNeighborTimeout();
};

#endif // RIP_H
