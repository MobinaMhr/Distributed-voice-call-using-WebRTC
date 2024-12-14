#ifndef ROUTER_H
#define ROUTER_H

#include "../Node/node.h"
#include "../Port/Port.h"
#include "routingtable.h"
#include <QString>
#include <deque>


class Router : public Node {
    Q_OBJECT

public:
    explicit Router(int id, const MacAddress &macAddress, int portCount, UT::IPVersion ipv, QThread *parent = nullptr);
    ~Router() override;

    void bufferPacket(const PacketPtr_t &packet);
    void addRoutingEntry(QSharedPointer<AbstractIP> &destinationIp, QSharedPointer<AbstractIP> &nextHopIp, QSharedPointer<Port> &port);
    void routePackets();
    QString ipv4Address() const override;
    QString ipv6Address() const override;
    PortPtr_t getIdlePort() override;
    std::vector<PortPtr_t> getPorts();
public Q_SLOTS:
    void receivePacket(const PacketPtr_t &packet) override;  // should called in receive packet slot !!

private:
    RoutingTable*                       m_routing_table;
    std::vector<PortPtr_t>              m_ports;
    UT::IPVersion                       m_ipvVersion;
    std::deque<PacketPtr_t>             m_buffer;
    IPv4_t                              m_ipv4Address;
    IPv6_t                              m_ipv6Address;

    bool isBufferAtCapacity();
    int findBufferPositionForPacket(UT::PacketType packetType);
};

#endif // ROUTER_H
