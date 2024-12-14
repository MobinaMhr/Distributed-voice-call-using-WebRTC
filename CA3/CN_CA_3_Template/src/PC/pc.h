#ifndef PC_H
#define PC_H

#include "../Node/Node.h"
#include "../IP/IP.h"
#include <QString>

class PC : public Node {
    Q_OBJECT

public:
    explicit PC(int id, const MacAddress &macAddress, QThread *parent = nullptr);
    ~PC() override;

    QString ipv4Address() const override;
    QString ipv6Address() const override;
    //void send(IPv4_t dest_ip);
    //void send(IPv6_t dest_ip); // create packets and send them;

public Q_SLOTS:
    void receivePacket(const PacketPtr_t &packet) override; // should called in receive packet slot !!

private:
    IPv4_t m_ipv4Address;
    IPv6_t m_ipv6Address;
    PortPtr_t m_port;
};

#endif // PC_H
