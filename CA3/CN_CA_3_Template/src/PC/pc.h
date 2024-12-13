#ifndef PC_H
#define PC_H

#include "../Node/Node.h"
#include <QString>

class PC : public Node {
    Q_OBJECT

public:
    explicit PC(int id, const MacAddress &macAddress, const IPv4_t &ipAddress, QThread *parent = nullptr);
    ~PC() override;

    QString ipAddress() const override;
    void receivePacket(const Packet &packet) override; // should called in receive packet slot !!
    void sendPacket(const Packet &packet) override;

private:
    IPv4_t m_ipAddress;
};

#endif // PC_H
