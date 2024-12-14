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
#include "../Packet/Packet.h"
#include <QString>
#include <vector>
#include "../Port/Port.h"

class Node : public QThread
{
    Q_OBJECT

public:
    explicit Node(int id, const MacAddress &macAddress, QThread *parent = nullptr);
    ~Node() override;
    // static Node *instance(int id, const MacAddress &macAddress, QThread *parent = nullptr);
    static void release();

    int id() const;
    QString name() const;
    virtual QString ipv4Address() const = 0;
    virtual QString ipv6Address() const = 0;
    QString macAddress() const;

protected:
    void run() override;

public Q_SLOTS:
    virtual void receivePacket(const PacketPtr_t &packet) = 0; // should called in receive packet slot !!

private Q_SLOTS:

Q_SIGNALS:
    void sendPacket(const PacketPtr_t &packet, int portNum) ;
private:
    inline static Node *m_self = nullptr;

    int m_id;
    MacAddress m_macAddress;
    QString m_name;
    UT::RoutingMode m_routingMode;
};


#endif // NODE_H
