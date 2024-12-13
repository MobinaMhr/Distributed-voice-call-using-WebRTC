#ifndef ROUTER_H
#define ROUTER_H

#include "../Node/node.h"
#include "../Port/Port.h"
#include "routingtable.h"
// #include "../DHCPServer/DHCPServer.h"
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

    void processPacket(const Packet &packet) override; // should called in receive packet slot !!
    void bufferPacket(const Packet &packet); // only used in router
    void addRoutingEntry(QSharedPointer<AbstractIP> &destinationIp, QSharedPointer<Port> &nextHop); // used by routing algorithems
    void routePacket(const AbstractIPHeader &header); // should emit send signal!!
    void routePackets(); // traverse over buffer to send packets
    // void printRoutingTable() const;

    void configurePort(int portIndex, const IPv4_t &ipAddress, const MacAddress &macAddress);

private:
    RoutingTable* m_routing_table;
    // std::unordered_map<AbstractIP, Port> m_routingTable; // Maps destination IP to next hop
    std::array<UT::PortState, 4> m_portStates;      // Four ports as QSharedPointer<Port>
    std::array<PortPtr_t, 4> m_ports;      // Four ports as QSharedPointer<Port>
    UT::IPVersion m_ipvVersion;
    std::deque<std::unique_ptr<Packet>> m_buffer;  // Infinite size buffer to hold packets temporarily
    // DHCPServer m_dhcpServer;              // Embedded DHCP server
};

#endif // ROUTER_H
