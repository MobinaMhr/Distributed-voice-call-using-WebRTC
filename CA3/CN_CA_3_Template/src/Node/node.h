#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QVector>
#include <QRandomGenerator>
#include <set>
#include "../IP/IP.h"
#include "../Globals/Globals.h"
#include "../MACAddress/macaddress.h"
#include <QString>
#include "../Port/Port.h"
#include <QFile>
#include <QTextStream>

const qint32 DHCP_TTL = 80;

class Node : public QThread
{
    Q_OBJECT

public:
    explicit Node(int id, const MacAddress &macAddress, int portCount, UT::IPVersion ipVersion, QThread *parent = nullptr);
    ~Node() override;
    // static Node *instance(int id, const MacAddress &macAddress, int portCount, QThread *parent = nullptr);
    static void release();

    // Getters
    int id() const;
    QString name() const;
    UT::IPVersion ipVersion() const;
    QString ipv4Address() const;
    QString ipv6Address() const;
    MacAddress macAddress() const;
    UT::NodeState state();
    IpPtr_t getIP2() {
        if (m_ipVersion == UT::IPVersion::IPv4) {
            return m_ipv4Address;
        }else {
            return m_ipv6Address;
        }
    }

    // Setters
    void setState(UT::NodeState state);
    void setIPVersion(UT::IPVersion ipVersion);

    void sendDiscovery();
    void log(const QString &message);

    virtual void processControlPacket(const PacketPtr_t &packet, uint8_t portNumber) = 0;
    virtual void processDataPacket(const PacketPtr_t &packet) = 0;
    virtual PortPtr_t getIdlePort() = 0;

protected:
    UT::IPVersion   m_ipVersion;
    IPv4Ptr_t       m_ipv4Address;
    IPv6Ptr_t       m_ipv6Address;
    bool            m_hasIP;
    QFile           m_logFile;

    void setIP(QString sugestedIP, QString mask);
    void run() override;
    bool isPacketMine(const PacketPtr_t &packet) {
        bool isMine = false;
        UT::IPVersion packetIpVersion = packet->ipVersion();

        /// Add BroadCast
        /// Add MultiCast

        if (packetIpVersion == UT::IPVersion::IPv4) {
            if (m_ipv4Address->toString() == packet->ipv4Header().destIp())
                isMine = true;
        } else if (packetIpVersion == UT::IPVersion::IPv6) {
            if (m_ipv6Address->toString() == packet->ipv6Header().destIp())
                isMine = true;
        }
        return isMine;
    }

public Q_SLOTS:
    virtual void receivePacket(const PacketPtr_t &packet, uint8_t portNumber) = 0;
    virtual void getIP() = 0;

private Q_SLOTS:

Q_SIGNALS:
    void sendPacket(const PacketPtr_t &packet, int portNum) ;
protected:
    inline static Node *m_self = nullptr;

    int             m_id;
    MacAddress      m_macAddress;
    QString         m_name;
    UT::RoutingMode m_routingMode;
    UT::NodeState   m_state;
};


#endif // NODE_H
