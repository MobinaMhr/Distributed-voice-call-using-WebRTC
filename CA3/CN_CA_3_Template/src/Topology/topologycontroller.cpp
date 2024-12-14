#include "TopologyController.h"
#include <QDebug>

TopologyController::TopologyController(int routerBufferSize, QObject *parent) :
    QObject(parent),
    m_topologyBuilder(new TopologyBuilder(routerBufferSize, this)),
    m_isActive(false) {}

TopologyController::~TopologyController() {
    delete m_topologyBuilder;
}

void TopologyController::updateTopology() {
    switch (m_activeTopologyType) {
        case UT::TopologyType::Mesh:
            m_topologyBuilder->moveToMeshTopology();
            break;
        case UT::TopologyType::Torus:
            m_topologyBuilder->moveToTorusTopology();
            break;
        case UT::TopologyType::RingStar:
            m_topologyBuilder->moveToRingStarTopology();
            break;
        case UT::TopologyType::None:
        default:
            qWarning() << "Invalid topology type!";
            return;
    }
}

void TopologyController::setTopologyType(UT::TopologyType topologyType) {
    m_activeTopologyType = topologyType;
}

void TopologyController::initializeTopology(UT::TopologyType topologyType, int routerCount, UT::IPVersion ipVersion, int offset, int portCount) {
    m_topologyBuilder->initializeRouters(routerCount, ipVersion, offset, portCount);
    setTopologyType(topologyType);
    updateTopology();
    m_currentTopology = m_topologyBuilder->routers();

    Q_EMIT topologyChanged(m_activeTopologyType);
}

QVector<QSharedPointer<Router>> TopologyController::getCurrentTopology() const {
    return m_currentTopology;
}

void TopologyController::monitorTopology() {
    if (!m_topologyBuilder || !m_isActive) return;

    qDebug() << "Monitoring topology...";
    for (const auto &node : m_topologyBuilder->routers()) {
        qDebug() << "Node" << node->id() << "is running.";
    }
}

void TopologyController::startSimulation() {
    Q_EMIT simulationStarted();
}

void TopologyController::stopSimulation() {
    Q_EMIT simulationStopped();
}

void TopologyController::resetTopology() {
    m_currentTopology.clear();
    m_activeTopologyType = UT::TopologyType::None;
}

void TopologyController::activateNodes() {
    if (!m_topologyBuilder) return;

    m_isActive = true;
    qDebug() << "Nodes activated.";
    for (const auto &node : m_topologyBuilder->routers()) {
        node->start();
    }
}

void TopologyController::deactivateNodes() {
    if (!m_topologyBuilder) return;

    m_isActive = false;
    qDebug() << "Nodes deactivated.";
    for (const auto &node : m_topologyBuilder->routers()) {
        node->quit();
        node->wait();
    }
}

QVector<QSharedPointer<PC>> TopologyController::getPcs(int count, int offset, UT::IPVersion ipVersion, int portCount) {
    m_personalComputers = m_topologyBuilder->getPCs(count, offset, ipVersion, portCount);
    return m_personalComputers;
}
