#ifndef OSPF_H
#define OSPF_H

#include <QObject>
#include <QMap>
#include <QTimer>
#include <QSet>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QVector>
#include "../Router/routingtable.h"

const int OSPF_TTL = 16;
const QString HELLO = "hello";
const QString LSA = "lsa";
const QString DEFAULT_MASK = "255.255.255.0";
const QString PROTOCOL = "OSPF";
const int HELLO_INTERVAL = 10;
const int LSA_INTERVAL = 30;


class OSPF : public QObject {
    Q_OBJECT

public:
    explicit OSPF(IPv4Ptr_t routerIp, MacAddress routerMac, QObject *parent = nullptr);
    ~OSPF() override = default;

    void run();
    void handleOSPFPacket(const PacketPtr_t& packet, const QSharedPointer<Port> &port);
    bool isLSAReady();
    // Packet getLSAPacket();

private:
    RoutingTable* m_routingTable;
    IPv4Ptr_t m_routerIp;
    Packet m_lsaPacket;
    MacAddress m_routerMac;
    IPHv4_t m_routerIpv4Header;
    IPHv6_t m_routerIpv6Header;
    QMap<QString, QSet<QString>> m_topologyGraph;
    bool m_lsaIsReady;
    bool m_isFinished;
    int m_notUpdatedTimes;

    void processHelloPacket(const PacketPtr_t &packet, const QSharedPointer<Port> &port);
    void processLSAPacket(const PacketPtr_t &packet);
    void updateTopologyGraph(const QJsonObject &lsaData);
    void computeRoutingTable();

    QString generateHelloPayload();
    QString generateLSAPayload();

    QJsonObject parsePayload(const QString& payload);
    void updateRoutingTable();

Q_SIGNALS:
    void routingUpdated(RoutingTable routingTable);

private Q_SLOTS:
};


#endif // OSPF_H
