#ifndef TOPOLOGYCONTROLLER_H
#define TOPOLOGYCONTROLLER_H

#include "TopologyBuilder.h"
#include <QObject>
#include <QFile>
#include <QTextStream>

class TopologyController : public QObject {
    Q_OBJECT

public:
    explicit TopologyController(int id, int routerBufferSize, QObject *parent = nullptr);
    ~TopologyController() override;

    void setTopologyType(UT::TopologyType topologyType);
    void updateTopology();
    void initializeTopology(UT::TopologyType topologyType, int routerCount, UT::IPVersion ipVersion, int offset, int portCount);
    QVector<QSharedPointer<Router>> getCurrentTopology() const;
    void monitorTopology();

    void activateNodes();
    void deactivateNodes();
    QVector<QSharedPointer<PC>> getPcs(int count, int offset, UT::IPVersion ipVersion, int portCount);

Q_SIGNALS:
    void topologyChanged(UT::TopologyType topologyType);
    void simulationStarted();
    void simulationStopped();

public Q_SLOTS:
    void startSimulation();
    void stopSimulation();
    void resetTopology();

private:
    TopologyBuilder*                m_topologyBuilder;
    QVector<QSharedPointer<Router>> m_currentTopology;
    QVector<QSharedPointer<PC>>     m_personalComputers;
    UT::TopologyType                m_activeTopologyType;
    bool                            m_isActive;
    QFile                           m_logFile;
    void log(const QString &message);
};

#endif // TOPOLOGYCONTROLLER_H
