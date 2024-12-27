#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <QObject>
#include <QMap>
#include <QPair>
#include "../IP/IP.h"
#include "../Node/node.h"

class Dijkstra : public QObject
{
    Q_OBJECT
public:
    struct Edge {
        IpPtr_t destination;
        int cost;
    };

    struct NodeData {
        IpPtr_t ip;
        QVector<Edge> edges;
    };

    explicit Dijkstra(const QMap<IpPtr_t, QSharedPointer<NodeData>> &network, QObject *parent = nullptr);
    QMap<IpPtr_t, QPair<int, IpPtr_t>> calculateShortestPaths(const IpPtr_t &source);

private:
    QMap<IpPtr_t, QSharedPointer<NodeData>> m_network;
};

#endif // DIJKSTRA_H
