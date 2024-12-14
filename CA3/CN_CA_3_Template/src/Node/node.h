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

class Node : public QThread
{
    Q_OBJECT

public:
    explicit Node(int id, const MacAddress &macAddress, int portCount, UT::IPVersion ipVersion, QThread *parent = nullptr);
    ~Node() override;
    // static Node *instance(int id, const MacAddress &macAddress, int portCount, QThread *parent = nullptr);
    static void release();

    int id() const;
    QString name() const;
    QString ipv4Address() const;
    QString ipv6Address() const;
    QString macAddress() const;

    UT::NodeState state();
    void setState(UT::NodeState state);

    virtual PortPtr_t getIdlePort() = 0;

protected:
    UT::IPVersion   m_ipVersion;
    IPv4_t          m_ipv4Address;
    IPv6_t          m_ipv6Address;

    void run() override;

public Q_SLOTS:
    virtual void receivePacket(const PacketPtr_t &packet) = 0;

private Q_SLOTS:

Q_SIGNALS:
    void sendPacket(const PacketPtr_t &packet, int portNum) ;
private:
    inline static Node *m_self = nullptr;

    int             m_id;
    MacAddress      m_macAddress;
    QString         m_name;
    UT::RoutingMode m_routingMode;
    UT::NodeState   m_state;
};


#endif // NODE_H
