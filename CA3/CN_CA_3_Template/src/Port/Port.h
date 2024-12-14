#ifndef PORT_H
#define PORT_H

#include "../Packet/Packet.h"
#include <QObject>

const int BROADCAST_ON_ALL_PORTS = -1;

typedef QSharedPointer<Packet> PacketPtr_t;

class Port : public QObject
{
    Q_OBJECT

public:
    explicit Port(QObject *parent = nullptr);
    ~Port() override;
    Port(const Port &other);
    Port& operator=(const Port &other);

public: // Getters
    QString ipAddress();
    uint8_t number();
    UT::PortState state();

public: // Setters
    void setIpAddress(QString ipAddress);
    void setState(UT::PortState state);

Q_SIGNALS:
    void packetSent(const PacketPtr_t &data);
    void packetReceived(const PacketPtr_t &data);

public Q_SLOTS:
    void sendPacket(const PacketPtr_t &data, int portNumber);
    void receivePacket(const PacketPtr_t &data);

private:
    uint8_t         m_number;
    uint64_t        m_numberOfPacketsSent;
    QString         m_ipAddress; // TODO: should change to 2 different ips 1 ipv4 and 1 ipv6 and should be of type IP
    UT::PortState   m_state;
};

typedef QSharedPointer<Port> PortPtr_t;

#endif // PORT_H
