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

public: // Getters
    QString ipAddress();
    uint8_t number();

public: // Setters
    void setIpAddress(QString ipAddress);
    void setMacAddress(MacAddress macAddress);

Q_SIGNALS:
    void packetSent(const PacketPtr_t &data);
    void packetReceived(const PacketPtr_t &data);

public Q_SLOTS:
    void sendPacket(const PacketPtr_t &data);
    void receivePacket(const PacketPtr_t &data);

private:
    uint8_t m_number;
    uint64_t m_numberOfPacketsSent;
    QString m_ipAddress;
    MacAddress m_macAddress;
};

typedef QSharedPointer<Port> PortPtr_t;

#endif // PORT_H
