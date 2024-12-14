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

    void createMeshTopology(int rows, int columns); // TODO : should return a struct of nodes and port binding;
    void createTorusTopology(int rows, int columns);
    void createStarTopology(int numNodes);
    void createRingStarTopology(int numNodes);

    QVector<QSharedPointer<Router>> nodes();

private:
    QVector<QSharedPointer<Router>> m_nodes;
    QVector<PortPtr_t>              m_ports;
    PortBindingManager              m_portBindingManager;
    MacAddressGenerator*            m_macAddressGenerator;

    QSharedPointer<Router> createRouter(int id);
    void bindPorts(QSharedPointer<Router> node1, QSharedPointer<Router> node2);
    void unbindPorts(QSharedPointer<Router> node1, QSharedPointer<Router> node2);
};

#endif // TOPOLOGYBUILDER_H
