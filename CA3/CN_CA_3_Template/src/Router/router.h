#ifndef ROUTER_H
#define ROUTER_H

#include "../Node/node.h"
#include "../Port/Port.h"
#include "../DHCP/dhcp.h"
#include "routingtable.h"
#include <QString>
#include <deque>

class Router : public Node {
    Q_OBJECT

public:
    explicit Router(int id, const MacAddress &macAddress, int portCount, UT::IPVersion ipVersion, int bufferSize, QThread *parent = nullptr);
    ~Router() override;

    void bufferPacket(const PacketPtr_t &packet);
    void addRoutingEntry(IpPtr_t &destinationIp, IpPtr_t &nextHopIp, QSharedPointer<Port> &port);
    void routePackets();
    // QString ipv4Address() const override;
    // QString ipv6Address() const override;
    PortPtr_t getIdlePort() override;
    std::vector<PortPtr_t> getPorts();

    std::vector<QSharedPointer<Node>> neighbors();
    void processControlPacket(const PacketPtr_t &packet) override;
    void processDataPacket(const PacketPtr_t &packet) override;

public Q_SLOTS:
    void receivePacket(const PacketPtr_t &packet) override;
public:
    RoutingTable*                       m_routing_table;
private:
    std::vector<PortPtr_t>              m_ports;
    std::deque<PacketPtr_t>             m_buffer;
    int                                 m_bufferSize;

    std::vector<QSharedPointer<Node>>   m_neighbors;
    DHCP*                               m_dhcp;

    bool isBufferAtCapacity();
    int findBufferPositionForPacket(UT::PacketType packetType);
};

#endif // ROUTER_H
