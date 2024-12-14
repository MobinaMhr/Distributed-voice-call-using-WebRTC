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

public:
    explicit TopologyBuilder(QObject *parent = nullptr);
    ~TopologyBuilder() override;

    void unbindAllPorts();

    void createMeshTopology(int rows, int columns);
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
