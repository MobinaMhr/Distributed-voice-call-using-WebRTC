#ifndef ROUTER_H
#define ROUTER_H

#include "../Node/node.h"
#include "../Port/Port.h"
#include "routingtable.h"
// #include "../BGP/BGP.h"
#include <QString>
#include <unordered_map>
#include <deque>
#include <memory>

class Router : public Node {
    Q_OBJECT

public:
    explicit Router(int id, const MacAddress &macAddress, QThread *parent = nullptr);
    ~Router() override;

    void receivePacket(const Packet &packet) override;  // should called in receive packet slot !!
    void sendPacket(const Packet &packet) override;     // should emit send signal!!

    void bufferPacket(const Packet &packet);
    void addRoutingEntry(QSharedPointer<AbstractIP> &destinationIp, QSharedPointer<Port> &nextHop); // used by routing algorithems
    void routePackets();

    void configurePort(int portIndex, const IPv4_t &ipAddress, const MacAddress &macAddress);

private:
    RoutingTable*                       m_routing_table;
    std::array<UT::PortState, 4>        m_portStates;
    std::array<PortPtr_t, 4>            m_ports;
    UT::IPVersion                       m_ipvVersion;
    std::deque<std::unique_ptr<Packet>> m_buffer;
};

#endif // ROUTER_H
