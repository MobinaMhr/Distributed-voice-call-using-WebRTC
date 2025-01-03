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
#include <QHash>
#include "../Router/routingtable.h"

const int OSPF_TTL = 4*4*4*4*4*4;
const QString LSA = "lsa";
const QString OSPF_PROTOCOL = "OSPF";
const int OSPF_FINISH_THRESHOLD = 50;
const QString DUMMY_NODE = "dummy";

struct NodeData
{
    QString node;
    QString parent;
    PortPtr_t port;

    NodeData(QString _node, QString _parent, PortPtr_t _port)
        : node(_node), parent(_parent), port(_port) {}

    NodeData() : node(DUMMY_NODE){}

    NodeData& operator=(const NodeData& other) {
        if (this != &other) {
            node = other.node;
            port = other.port;
            parent = other.parent;
        }
        return *this;
    }

    bool operator<(const NodeData& other) const {
        return node < other.node;  // Compare based on node, or adjust logic as needed
    }

    // Overload the equality operator
    bool operator==(const NodeData& other) const {
        return node == other.node && parent == other.parent;
    }
};

inline uint qHash(const NodeData& key, uint seed = 0) {
    return qHash(key.node, seed) ^ qHash(key.parent, seed) ;
}

class OSPF : public QObject {
    Q_OBJECT

public:
    explicit OSPF(IPv4Ptr_t routerIp, MacAddress routerMac, QObject *parent = nullptr);
    ~OSPF() override = default;

    void run();
    void handleOSPFPacket(const PacketPtr_t& packet, const QSharedPointer<Port> &port);
    bool isLSAReady();
    Packet getlsaPacket();
    // Packet getLSAPacket();

private:
    RoutingTable* m_routingTable;
    IPv4Ptr_t m_routerIp;
    Packet m_lsaPacket;
    MacAddress m_routerMac;
    IPHv4_t m_routerIpv4Header;
    IPHv6_t m_routerIpv6Header;
    QMap<QString, QSet<NodeData>> m_topologyGraph;
    bool m_lsaIsReady;
    bool m_isFinished;
    int m_notUpdatedTimes;

    void processHelloPacket(const PacketPtr_t &packet, const QSharedPointer<Port> &port);
    void processLSAPacket(const PacketPtr_t &packet, const QSharedPointer<Port> &port);
    void updateTopologyGraph(const QJsonObject &lsaData, const QSharedPointer<Port> &ports);
    void updateLsaPacket();
    void computeRoutingTable();

    QString generateHelloPayload();
    QString generateLSAPayload();

    QJsonObject parsePayload(const QString& payload);
    void updateRoutingTable();
    NodeData findShortestPath(QMap<NodeData, int> tempList);
    NodeData pathToSameNode(QString node, QMap<NodeData, int> tempList);

Q_SIGNALS:
    void routingFinished(RoutingTable routingTable);

private Q_SLOTS:
};


#endif // OSPF_H
