#ifndef TOPOLOGYBUILDER_H
#define TOPOLOGYBUILDER_H

#include "../PortBindingManager/PortBindingManager.h"
#include "../MACAddress/macaddressgenerator.h"
#include "../Router/router.h"
#include <QObject>
#include <QVector>
#include <QMap>
#include <memory>

class TopologyBuilder : public QObject {
    Q_OBJECT

public:// up = 0, right = 1 , down = 2 , left = 3 -> MAGICNUMBER
    explicit TopologyBuilder(QObject *parent = nullptr);
    ~TopologyBuilder() override; // TODO: get IPV as input

    void unbindAllPorts();

    void initializeRouters(int count, int offset = 0);
    void moveToMeshTopology(int rows, int columns); // TODO : should return a struct of nodes and port binding;
    void moveToTorusTopology(int rows, int columns);
    void moveToStarTopology();
    void moveToRingStarTopology();

    QVector<QSharedPointer<Router>> nodes();

private:
    int                             m_offset;
    int                             m_routerCount;
    QVector<QSharedPointer<Router>> m_nodes;
    QVector<PortPtr_t>              m_ports;
    PortBindingManager              m_portBindingManager;
    MacAddressGenerator*            m_macAddressGenerator;

    QSharedPointer<Router> createRouter(int id, int portCount);
    void bindPorts(QSharedPointer<Router> node1, QSharedPointer<Router> node2);
    void unbindPorts(QSharedPointer<Router> node1, QSharedPointer<Router> node2);
};

#endif // TOPOLOGYBUILDER_H
