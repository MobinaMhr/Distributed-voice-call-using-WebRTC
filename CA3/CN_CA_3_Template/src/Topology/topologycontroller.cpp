#include "TopologyController.h"
#include <QDebug>

TopologyController::TopologyController(QObject *parent) :
    QObject(parent),
    m_topologyBuilder(new TopologyBuilder()),
    m_isActive(false) {}

TopologyController::~TopologyController() {}

void TopologyController::changeTopologyTo(UT::TopologyType topologyType) {
    /// TODO,,,
}

void TopologyController::initializeTopology(UT::TopologyType topologyType, int numNodes) {
    if (!m_topologyBuilder) m_topologyBuilder = new TopologyBuilder();

    int rows = numNodes / 2;
    int columns = numNodes / 2;

    switch (topologyType) {
        case UT::TopologyType::Mesh:
            m_topologyBuilder->createMeshTopology(rows, columns);
            break;
        case UT::TopologyType::Torus:
            m_topologyBuilder->createTorusTopology(rows, columns);
            break;
        case UT::TopologyType::RingStar:
            m_topologyBuilder->createRingStarTopology(numNodes);
            break;
        case UT::TopologyType::None:
        default:
            qWarning() << "Invalid topology type!";
            return;
    }

    m_currentTopology = m_topologyBuilder->nodes();
    m_activeTopologyType = topologyType;
    Q_EMIT topologyChanged(topologyType);
}

QVector<QSharedPointer<Router>> TopologyController::getCurrentTopology() const {
    return m_currentTopology;
}

void TopologyController::monitorTopology() {
    if (!m_topologyBuilder || !m_isActive) return;

    qDebug() << "Monitoring topology...";
    for (const auto &node : m_topologyBuilder->nodes()) {
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
    for (const auto &node : m_topologyBuilder->nodes()) {
        node->start();
    }
}

void TopologyController::deactivateNodes() {
    if (!m_topologyBuilder) return;

    m_isActive = false;
    qDebug() << "Nodes deactivated.";
    for (const auto &node : m_topologyBuilder->nodes()) {
        node->quit();
        node->wait();
    }
}
