#ifndef TOPOLOGYBUILDER_H
#define TOPOLOGYBUILDER_H

#include "../PortBindingManager/PortBindingManager.h"
#include "../MACAddress/macaddressgenerator.h"
#include "../Router/router.h"
#include "../PC/pc.h"
#include <QObject>
#include <QVector>
#include <QMap>
#include <memory>
#include <QFile>
#include <QTextStream>

class TopologyBuilder : public QObject {
    Q_OBJECT

public:
    explicit TopologyBuilder(int id, int routerBufferSize, QObject *parent = nullptr);
    ~TopologyBuilder() override;

    void resetBindings();

    void initializeRouters(int routerCount, UT::IPVersion ipVersion, int offset = 0, int portCount = 4);
    void moveToMeshTopology();
    void moveToTorusTopology();
    void moveToStarTopology();
    void moveToRingStarTopology();

    QVector<QSharedPointer<Router>> routers();
    QVector<QSharedPointer<PC>> getPCs(int count, int offset, UT::IPVersion ipVersion, int portCount);

private:
    int                             m_offset;
    int                             m_routerCount;
    int                             m_rows;
    int                             m_columns;
    int                             m_routerBufferSize;
    QVector<QSharedPointer<Router>> m_routers;
    PortBindingManager*             m_portBindingManager;
    MacAddressGenerator*            m_macAddressGenerator;
    QFile                           m_logFile;

    QSharedPointer<Router> createRouter(int id, int portCount, UT::IPVersion ipVersion);
    QSharedPointer<PC> createPC(int id, int portCount, UT::IPVersion ipVersion);
    bool bindPorts(PortPtr_t port1, PortPtr_t port2);
    bool unbindPorts(PortPtr_t port1, PortPtr_t port2);
    void log(const QString &message);
};

#endif // TOPOLOGYBUILDER_H
