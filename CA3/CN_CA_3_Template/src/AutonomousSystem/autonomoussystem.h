#ifndef AUTONOMOUSSYSTEM_H
#define AUTONOMOUSSYSTEM_H

#include "../Topology/topologycontroller.h"
#include <QObject>
#include <QVector>
#include <QSharedPointer>
#include <QMap>

class AutonomousSystem : public QObject
{
    Q_OBJECT

public:
    explicit AutonomousSystem(int routerCount, int pcCount, int routerOffset, int pcOffset,
                              UT::TopologyType topology_type, QObject *parent = nullptr);
    ~AutonomousSystem() override;

    void startRoutingProtocol();
    void stopRoutingProtocol();

    void assignIPAddresses();
    void monitorNetwork();
    void resetAS();
    void configAS(UT::TopologyType topology_type);

    QVector<QSharedPointer<Router>> routers();

Q_SIGNALS:
    void routingProtocolStarted();
    void routingProtocolStopped();

public Q_SLOTS:
    void startSimulation();
    void stopSimulation();

private:
    int                             m_id;
    int                             m_routerCount;
    int                             m_pcCount;
    int                             m_routerOffset;
    int                             m_pcOffset;
    bool                            m_isSimulationActive;
    UT::TopologyType                m_topologyType;
    QVector<QSharedPointer<Router>> m_routers;
    QVector<QSharedPointer<Router>> m_as_gateways;
    QVector<QSharedPointer<Router>> m_userGateways;
    QVector<QSharedPointer<Router>> m_brokenRouters;
    QSharedPointer<Router>          m_dhcpServer;
    TopologyController*             m_topologyController;
    UT::IPVersion                   m_ipVersion;

    void initializeAS();
    void initializePCs(UT::IPVersion ipVersion, int offset, int portCount = 1);
};

#endif    // AUTONOMOUSSYSTEM_H
