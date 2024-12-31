#ifndef ROUTER_H
#define ROUTER_H

#include "../Node/node.h"
#include "../Port/Port.h"
#include "../DHCP/dhcp.h"
#include "routingtable.h"
#include <QString>
#include <deque>
#include <QJsonObject>
#include <QJsonDocument>

const QString DEFAULT_MASK = "255.255.255.0";

class Router : public Node {
    Q_OBJECT

public:
    explicit Router(int id, const MacAddress &macAddress, int portCount, UT::IPVersion ipVersion, int bufferSize, QThread *parent = nullptr);
    ~Router() override;

    void bufferPacket(const PacketPtr_t &packet);
    void addRoutingEntry(IpPtr_t &destinationIp, IpPtr_t &nextHopIp, QSharedPointer<Port> &port, int metric);
    void routePackets();
    // QString ipv4Address() const override;
    // QString ipv6Address() const override;
    PortPtr_t getIdlePort() override;
    std::vector<PortPtr_t> getPorts();
    void setDhcp(int asNumber);

    std::vector<QSharedPointer<Node>> neighbors();
    void processControlPacket(const PacketPtr_t &packet, uint8_t portNumber) override;
    void processDataPacket(const PacketPtr_t &packet) override;

public Q_SLOTS:
    void receivePacket(const PacketPtr_t &packet, uint8_t portNumber) override;
    virtual void getIP() override;
public:
    RoutingTable*                       m_routing_table;
private:
    std::vector<PortPtr_t>              m_ports;
    std::deque<PacketPtr_t>             m_buffer;
    int                                 m_bufferSize;

    std::vector<QSharedPointer<Node>>   m_neighbors;
    DHCP*                               m_dhcp;

    void handleDhcpDiscovery(PacketPtr_t packet);
    void handleDhcpOffer(PacketPtr_t packet);
    void handleDhcpReq(PacketPtr_t packet);
    void handleDhcpAck(PacketPtr_t packet);
    QString createDhcpAckBody(PacketPtr_t packet);

    bool isBufferAtCapacity();
    int findBufferPositionForPacket(UT::PacketType packetType);
};

#endif // ROUTER_H
