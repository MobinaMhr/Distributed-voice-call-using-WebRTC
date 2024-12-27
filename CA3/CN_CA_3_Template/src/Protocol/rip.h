#ifndef RIP_OSPF_H
#define RIP_OSPF_H

#include <QObject>
#include <QMap>
#include <QSharedPointer>
#include <QTimer>
#include <QVector>
#include "../Router/router.h"

class RIP : public QObject
{
    Q_OBJECT

public:
    explicit RIP(QObject *parent = nullptr);
    ~RIP() override;

    void startProtocol(const QVector<QSharedPointer<Router>> &routers);
    void stopProtocol();

Q_SIGNALS:
    void routingStable();

private:
    QVector<QSharedPointer<Router>> m_routers;
    QMap<int, QMap<int, int>> m_routingTables; // Router ID -> (Destination ID -> Cost)
    QTimer m_updateTimer;

    void initializeRoutingTables();
    void sendRoutingUpdates();
    void receiveRoutingUpdate(int routerId, const QMap<int, int> &update);
    void finalizeRoutingTables();
};

class OSPF : public QObject
{
    Q_OBJECT

public:
    explicit OSPF(QObject *parent = nullptr);
    ~OSPF() override;

    void startProtocol(const QVector<QSharedPointer<Router>> &routers);
    void stopProtocol();

Q_SIGNALS:
    void routingStable();

private:
    QVector<QSharedPointer<Router>> m_routers;
    QMap<int, QMap<int, int>> m_linkStateDatabases; // Router ID -> (Neighbor ID -> Cost)
    QTimer m_updateTimer;

    void initializeLSAs();
    void sendLSAs();
    void receiveLSA(int routerId, const QMap<int, int> &lsa);
    void calculateShortestPaths();
    void finalizeRoutingTables();
};

#endif // RIP_OSPF_H
