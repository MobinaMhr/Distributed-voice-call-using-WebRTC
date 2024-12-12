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
    void sendPacket(const QString &destinationIP, const QString &data);
    void processPacket(const Packet &packet) override;

private:
    IPv4_t m_ipAddress;
};

#endif // PC_H
