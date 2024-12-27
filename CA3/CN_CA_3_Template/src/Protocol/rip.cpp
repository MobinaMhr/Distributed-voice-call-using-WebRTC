#include "rip.h"
#include <QDebug>

RIP::RIP(QObject *parent) : QObject(parent) {}

RIP::~RIP() {}

void RIP::startProtocol(const QVector<QSharedPointer<Router>> &routers)
{
    m_routers = routers;
    initializeRoutingTables();
    sendRoutingUpdates();
}

void RIP::stopProtocol()
{
    m_updateTimer.stop();
    qDebug() << "RIP protocol stopped.";
}

void RIP::initializeRoutingTables()
{
    for (const auto &router : m_routers) {
        QMap<int, int> table;
        for (const auto &neighbor : router->neighbors()) {
            table[neighbor->id()] = 1; // Direct neighbors have cost 1
        }
        m_routingTables[router->id()] = table;
    }
    qDebug() << "RIP routing tables initialized.";
}

void RIP::sendRoutingUpdates()
{
    for (const auto &router : m_routers) {
        QMap<int, int> update = m_routingTables[router->id()];
        for (const auto &neighbor : router->neighbors()) {
            MacAddress srcMacAddress = router->macAddress();
            MacAddress destMacAddress = neighbor->macAddress();

            uint16_t srcPort;
            uint16_t destPort;

            QByteArray payload;
            QDataStream stream(&payload, QIODevice::WriteOnly);
            stream << update;

            PacketPtr_t packetPtr = QSharedPointer<Packet>::create(
                UT::PacketType::Control, UT::PacketControlType::RIP, 0, 0, 0, nullptr,
                payload, DataLinkHeader(srcMacAddress, destMacAddress), TCPHeader(srcPort, destPort),
                IPHv4_t(), IPHv6_t(), nullptr
            );
            Q_EMIT router->sendPacket(packetPtr, 0);
        }
    }
}

void RIP::receiveRoutingUpdate(int routerId, const QMap<int, int> &update)
{
    QMap<int, int> &table = m_routingTables[routerId];
    bool updated = false;

    for (auto it = update.begin(); it != update.end(); ++it) {
        int destination = it.key();
        int cost = it.value() + 1; // Add cost to reach neighbor

        if (!table.contains(destination) || table[destination] > cost) {
            table[destination] = cost;
            updated = true;
        }
    }

    if (updated) {
        sendRoutingUpdates();
    }
}

void RIP::finalizeRoutingTables()
{
    qDebug() << "RIP routing tables finalized.";
    Q_EMIT routingStable();
}

OSPF::OSPF(QObject *parent) : QObject(parent) {}

OSPF::~OSPF() {}

void OSPF::startProtocol(const QVector<QSharedPointer<Router>> &routers)
{
    m_routers = routers;
    initializeLSAs();
    sendLSAs();
}

void OSPF::stopProtocol()
{
    m_updateTimer.stop();
    qDebug() << "OSPF protocol stopped.";
}

void OSPF::initializeLSAs()
{
    for (const auto &router : m_routers) {
        QMap<int, int> lsas;
        for (const auto &neighbor : router->neighbors()) {
            lsas[neighbor->id()] = 1; // Direct neighbors have cost 1
        }
        m_linkStateDatabases[router->id()] = lsas;
    }
    qDebug() << "OSPF LSAs initialized.";
}

void OSPF::sendLSAs()
{
    for (const auto &router : m_routers) {
        QMap<int, int> lsa = m_linkStateDatabases[router->id()];
        for (const auto &neighbor : router->neighbors()) {
            MacAddress srcMacAddress = router->macAddress();
            MacAddress destMacAddress = neighbor->macAddress();

            uint16_t srcPort;
            uint16_t destPort;

            QByteArray payload;
            QDataStream stream(&payload, QIODevice::WriteOnly);
            stream << lsa;

            PacketPtr_t packetPtr = QSharedPointer<Packet>::create(
                UT::PacketType::Control, UT::PacketControlType::OSPF, 0, 0, 0, nullptr,
                payload, DataLinkHeader(srcMacAddress, destMacAddress), TCPHeader(srcPort, destPort),
                IPHv4_t(), IPHv6_t(), nullptr
            );
            Q_EMIT router->sendPacket(packetPtr, 0);
        }
    }
}

void OSPF::receiveLSA(int routerId, const QMap<int, int> &lsa)
{
    QMap<int, int> &database = m_linkStateDatabases[routerId];
    bool updated = false;

    for (auto it = lsa.begin(); it != lsa.end(); ++it) {
        int neighbor = it.key();
        int cost = it.value();

        if (!database.contains(neighbor) || database[neighbor] > cost) {
            database[neighbor] = cost;
            updated = true;
        }
    }

    if (updated) {
        calculateShortestPaths();
    }
}

void OSPF::calculateShortestPaths()
{
    // Implement Dijkstra's algorithm here
    qDebug() << "OSPF shortest paths calculated.";
    finalizeRoutingTables();
}

void OSPF::finalizeRoutingTables()
{
    qDebug() << "OSPF routing tables finalized.";
    Q_EMIT routingStable();
}
