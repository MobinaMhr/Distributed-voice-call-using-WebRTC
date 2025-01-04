#ifndef ROUTER_H
#define ROUTER_H

#include "../Node/node.h"
#include "../Port/Port.h"
#include "../DHCP/dhcp.h"
#include "routingtable.h"
#include "../Protocol/rip.h"
#include "../Protocol/ospf.h"
#include <QString>
#include <deque>
#include <QJsonObject>
#include <QJsonDocument>

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
    void route(UT::PacketControlType protocol);// my change to slot

    std::vector<QSharedPointer<Node>> neighbors();
    void processControlPacket(const PacketPtr_t &packet, uint8_t portNumber) override;
    void processDataPacket(const PacketPtr_t &packet) override;
    void runBgp();

Q_SIGNALS:
    void routingFinished(int id);

public Q_SLOTS:
    void receivePacket(const PacketPtr_t &packet, uint8_t portNumber) override;
    void finishRouting(RoutingTable routingTable);
    virtual void getIP() override;
    void handleTick(int cycleNumebt);
public:
    RoutingTable*                       m_routing_table;
private:
    std::vector<PortPtr_t>              m_ports;
    std::deque<PacketPtr_t>             m_buffer;
    int                                 m_bufferSize;

    std::vector<QSharedPointer<Node>>   m_neighbors;
    DHCP*                               m_dhcp;
    bool                                m_isRouting;
    bool                                m_isBgpWorking;
    UT::PacketControlType               m_protocol;
    RIP                                 *m_rip;
    OSPF                                *m_ospf;

    void handleDhcpDiscovery(PacketPtr_t packet);
    void handleDhcpOffer(PacketPtr_t packet);
    void handleDhcpReq(PacketPtr_t packet);
    void handleDhcpAck(PacketPtr_t packet);
    QString createDhcpAckBody(PacketPtr_t packet);
    void handleRoutingPacket();
    void handleOspfPacket(PacketPtr_t packet, int portNumber);

    bool isBufferAtCapacity();
    int findBufferPositionForPacket(UT::PacketType packetType);
};

#endif // ROUTER_H
