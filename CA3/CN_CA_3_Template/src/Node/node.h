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

class Node : public QThread
{
    Q_OBJECT

public:
    explicit Node(int id, const MacAddress &macAddress, QThread *parent = nullptr);
    ~Node() override;
    // static Node *instance(int id, const MacAddress &macAddress, QThread *parent = nullptr);
    static void release();

    virtual void processPacket(const Packet &packet) = 0;
    int id() const;
    QString name() const;
    virtual QString ipAddress() const = 0;
    QString macAddress() const;

protected:
    void run() override;

public Q_SLOTS:

private Q_SLOTS:

Q_SIGNALS:

private:
    inline static Node *m_self = nullptr;

    int m_id;
    MacAddress m_macAddress;
    QString m_name;
};


#endif // NODE_H
