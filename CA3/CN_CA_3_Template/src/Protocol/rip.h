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
const QString HELLO = "hello";
const QString UPDATE = "update";
const QString DEFAULT_MASK = "255.255.255.0";
const QString PROTOCOL = "RIP";
const int NEIGHBOR_COST = 1;
const int FINISH_THRESHOLD = 12;

class RIP : public QObject {
    Q_OBJECT

public:
    explicit RIP(IPv4Ptr_t routerIp, MacAddress routerMac, QObject* parent = nullptr);
    ~RIP() override = default;
    void run();

    void handleRIPPacket(const PacketPtr_t& packet, const QSharedPointer<Port> &port);
    bool isUpdateReady();
    Packet getUpdatePacket();

private:
    RoutingTable* m_routingTable;
    IPv4Ptr_t     m_currentRouterIp;
    Packet m_updatePacket;
    MacAddress m_routerMacAddress;
    IPHv4_t m_routerIpv4Header;
    IPHv6_t m_routerIpv6Header;
    bool m_updateIsReady;
    bool m_isFinished;
    int m_notUpdatedTimes;

    QString generateUpdatePayload(QString type, QVector<IpPtr_t> nodes, QVector<int> costs);
    QString generateUpdatePayload(QString type, QVector<QString> nodes, QVector<int> costs);
    QJsonObject extractUpdatePayloadJson(const QString& jsonString);
    QVector<QString> extractNodes(QJsonObject update);
    QVector<int> extractCosts(QJsonObject update);
    void handleHello(const PacketPtr_t &packet, const QSharedPointer<Port> &port);
    void handleUpdate(const PacketPtr_t &packet, const QVector<QString> &nodes, const QVector<int> costs,
                                    const QSharedPointer<Port> &port);
    QString generateUpdatePacket();

    void updateRoutingTable(const QJsonObject& routingData, const IpPtr_t& neighborIp, int neighborMetric);

    static QJsonObject createRoutingData(const RoutingTable* routingTable);

Q_SIGNALS:
    void routingFinished(RoutingTable routingTable);

private Q_SLOTS:
    // void onNeighborTimeout();
};

#endif // RIP_H
