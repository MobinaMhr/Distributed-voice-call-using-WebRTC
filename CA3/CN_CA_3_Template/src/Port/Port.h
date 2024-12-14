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
    explicit Port(uint8_t number, QObject *parent = nullptr);
    ~Port() override;
    Port(const Port &other);
    // Port& operator=(const Port &other);

public: // Getters
    uint8_t number();
    UT::PortState state();
    QString ipAddress();

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
    UT::IPVersion   m_ipVersion;
    IPv4_t          m_ipv4Address;
    IPv6_t          m_ipv6Address;
    UT::PortState   m_state;

    QString ipv6Address() const;
    QString ipv4Address() const;
};

typedef QSharedPointer<Port> PortPtr_t;

#endif // PORT_H
