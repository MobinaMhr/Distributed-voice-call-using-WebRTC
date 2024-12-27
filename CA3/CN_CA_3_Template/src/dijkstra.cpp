#include "dijkstra.h"
#include <QSet>
#include <QDebug>

Dijkstra::Dijkstra(const QMap<IpPtr_t, Node> &network)
    : m_network(network) {}

QMap<IpPtr_t, QPair<int, IpPtr_t>> Dijkstra::calculateShortestPaths(const IpPtr_t &source) {
    QMap<IpPtr_t, int> distances;
    QMap<IpPtr_t, IpPtr_t> previousNodes;
    QSet<IpPtr_t> unvisited;

    // Initialize distances
    for (const auto &ip : m_network.keys()) {
        distances[ip] = std::numeric_limits<int>::max();
        unvisited.insert(ip);
    }
    distances[source] = 0;

    while (!unvisited.isEmpty()) {
        // Find the node with the smallest distance
        IpPtr_t currentNode = *std::min_element(
          unvisited.begin(), unvisited.end(),
          [&distances](const IpPtr_t &a, const IpPtr_t &b) {
              return distances[a] < distances[b];
          });

        unvisited.remove(currentNode);

        // Check if we reached an unreachable node
        if (distances[currentNode] == std::numeric_limits<int>::max()) {
            break;
        }

        // Update distances to neighbors
        for (const auto &edge : m_network[currentNode].edges) {
            if (unvisited.contains(edge.destination)) {
                int newDist = distances[currentNode] + edge.cost;
                if (newDist < distances[edge.destination]) {
                    distances[edge.destination] = newDist;
                    previousNodes[edge.destination] = currentNode;
                }
            }
        }
    }

    // Convert results into a final map
    QMap<IpPtr_t, QPair<int, IpPtr_t>> result;
    for (const auto &ip : distances.keys()) {
        result[ip] = qMakePair(distances[ip], previousNodes.value(ip, nullptr));
    }
    return result;
}
