#ifndef PC_H
#define PC_H

#include "../Node/Node.h"
#include "../IP/IP.h"
#include <QString>

class PC : public Node {
    Q_OBJECT

public:
    explicit PC(int id, const MacAddress &macAddress, int portCount, UT::IPVersion ipVersion, QThread *parent = nullptr);
    ~PC() override;

    // QString ipv4Address() const override;
    // QString ipv6Address() const override;
    PortPtr_t getIdlePort() override;
    //void send(IPv4_t dest_ip);
    //void send(IPv6_t dest_ip); // create packets and send them;
    void processControlPacket(const PacketPtr_t &packet) override;
    void processDataPacket(const PacketPtr_t &packet) override;

public Q_SLOTS:
    void receivePacket(const PacketPtr_t &packet) override;

private:
    PortPtr_t       m_port;
};

#endif // PC_H
