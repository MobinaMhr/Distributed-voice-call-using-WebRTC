#ifndef ROUTER_H
#define ROUTER_H

#include "../Node/node.h"
#include "../Port/Port.h"
// #include "../DHCPServer/DHCPServer.h"
// #include "../BGP/BGP.h"
#include <QString>
#include <QMap>
#include <deque>
#include <memory>

class Router : public Node {
    Q_OBJECT

public:
    explicit Router(int id, const MacAddress &macAddress, QThread *parent = nullptr);
    ~Router() override;
    QString ipAddress() const override;
    void processPacket(const Packet &packet) override;

    void addRoutingEntry(const QString &destination, const QString &nextHop);
    void routePacket(const AbstractIPHeader &header);

    void printRoutingTable() const;

    void configurePort(int portIndex, const IPv4_t &ipAddress, const MacAddress &macAddress);

private:
    QMap<QString, QString> m_routingTable; // Maps destination IP to next hop
    std::array<PortPtr_t, 4> m_ports;      // Four ports as QSharedPointer<Port>
    UT::IPVersion m_ipvVersion;
    std::deque<std::unique_ptr<Packet>> m_buffer;  // Infinite size buffer to hold packets temporarily
    // DHCPServer m_dhcpServer;              // Embedded DHCP server
};

#endif // ROUTER_H
