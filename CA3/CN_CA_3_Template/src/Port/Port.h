#ifndef PORT_H
#define PORT_H

#include "../Packet/Packet.h"
#include <QObject>

typedef QSharedPointer<Packet> PacketPtr_t;

class Port : public QObject
{
    Q_OBJECT

public:
    explicit Port(QObject *parent = nullptr);
    ~Port() override;
    // Copy Constructor
    Port(const Port &other);

    // Assignment Operator (optional, to complement the copy constructor)
    Port& operator=(const Port &other);

public: // Getters
    QString ipAddress();
    uint8_t number();

public: // Setters
    void setIpAddress(QString ipAddress);

Q_SIGNALS:
    void packetSent(const PacketPtr_t &data); // connected to receivePacket slot of bined port!!
    void packetReceived(const PacketPtr_t &data); // connected to receivePacket slot coresponding node!!

public Q_SLOTS:
    void sendPacket(const PacketPtr_t &data); // connected to coresponding node send signal!!
    void receivePacket(const PacketPtr_t &data); // connected to the bined port packetSent signal!!

private:
    uint8_t m_number;
    uint64_t m_numberOfPacketsSent;
    QString m_ipAddress; // TODO: should change to 2 different ips 1 ipv4 and 1 ipv6 and should be of type IP
};

typedef QSharedPointer<Port> PortPtr_t;

#endif // PORT_H
