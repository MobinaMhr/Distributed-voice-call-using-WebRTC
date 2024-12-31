# Protocol

# RIP

This class implements the Routing Information Protocol (RIP) for a router. 

RIP is a distance-vector routing protocol used in local and wide area networks. 

It ensures efficient routing by continuously updating the routing table based on received information and terminating the process when no further updates are necessary. 

The use of Qt framework facilitates JSON handling and signal-slot mechanisms, making the implementation robust and efficient.

The constructor class sets some instances of related pointers for the RiP algorithm.
```cpp
RIP::RIP(IPv4Ptr_t routerIp, MacAddress routerMac, QObject* parent)
    : QObject(parent),
    m_currentRouterIp(routerIp),
    m_updatePacket(UT::PacketType::Control, UT::PacketControlType::RIP, 1, 0, 0,
                   IPv4_t::createIpPtr("0.0.0.0", "255.255.255.0"), QByteArray(),
                   DataLinkHeader(MacAddress("00:00:00:00:00:00"), MacAddress("00:00:00:00:00:00")),
                   TCPHeader(0, 0), IPHv4_t(), IPHv6_t(), RIP_TTL, this),
    m_routerMacAddress(routerMac), m_updateIsReady(false), m_isFinished(false), m_notUpdatedTimes(0)
{
    m_routingTable = new RoutingTable(this);
    m_routerIpv4Header = *(new IPHv4_t());
    m_routerIpv4Header.setSourceIp(m_currentRouterIp);
    m_routerIpv6Header = *(new IPHv6_t());
    m_routerIpv6Header.setSourceIp(m_currentRouterIp->toIPv6());
}
```

The run method creates a HELLO packet with the bellow detail. This packet will be broadcasted in all ports of the router.
```cpp
void RIP::run()
{
    IpPtr_t fakeDest = IPv4_t::createIpPtr("255.255.255.255", "255.255.255.255");
    QByteArray payload;
    DataLinkHeader *dh = new DataLinkHeader(this->m_routerMacAddress, this->m_routerMacAddress);
    TCPHeader *th = new TCPHeader(BROADCAST_ON_ALL_PORTS, BROADCAST_ON_ALL_PORTS);
    Packet *hello = new Packet(UT::PacketType::Control, UT::PacketControlType::RIP,
                                       1, 0, 0, fakeDest, payload, *dh, *th, m_routerIpv4Header, m_routerIpv6Header,
                                       RIP_TTL);

    QVector<IpPtr_t> nodes = {};

    hello->storeStringInPayload(generateUpdatePayload(HELLO, nodes, {}));
    m_updatePacket = *hello;
    m_updateIsReady = true;
}
```

The `handleRIPPacket()` method processes incoming RIP packets. It extracts the update payload from the packet, determines the type of the update (either HELLO or UPDATE), and calls the appropriate handler method (handleHello or handleUpdate). It uses helper methods to extract nodes and costs from the update payload.
```cpp
void RIP::handleRIPPacket(const PacketPtr_t &packet, const QSharedPointer<Port> &port)
{
    QJsonObject update = extractUpdatePayloadJson(packet->readStringFromPayload());
    QString type = update["type"].toString();
    QVector<QString> nodes = extractNodes(update);
    QVector<int> costs = extractCosts(update);
    if (type == HELLO)
        handleHello(packet, port);
    else
        handleUpdate(packet, nodes, costs, port);
}
```

This method returns a boolean indicating whether an update is ready to be sent. It checks the m_updateIsReady member variable.
```cpp
bool RIP::isUpdateReady()
{
    return m_updateIsReady;
}
```

This method returns the update packet. If the routing process is not finished, it sets m_updateIsReady to false before returning the packet.
```cpp
Packet RIP::getUpdatePacket()
{
    if(!m_isFinished)
        m_updateIsReady = false;
    return m_updatePacket;
}
```

This method generates a JSON payload for an update. It takes the update type, a vector of nodes, and a vector of costs as input. It constructs a JSON object with these values and returns it as a compact JSON string.
```cpp
QString RIP::generateUpdatePayload(QString type, QVector<IpPtr_t> nodes, QVector<int> costs)
{
    QJsonObject jsonObject;
    jsonObject["type"] = type;

    QJsonArray nodesArray;
    for (const IpPtr_t& node : nodes) {
        if (node) {
            nodesArray.append(node->toString());
        }
    }
    jsonObject["nodes"] = nodesArray;

    QJsonArray costsArray;
    for (const auto& cost : costs) {
        costsArray.append(cost);
    }
    jsonObject["costs"] = costsArray;

    QJsonDocument jsonDoc(jsonObject);
    return QString(jsonDoc.toJson(QJsonDocument::Compact));
}
```

This method is similar to the previous one but takes a vector of node strings instead of IpPtr_t objects. It constructs a JSON object with the update type, nodes, and costs, and returns it as a compact JSON string.
```cpp
QString RIP::generateUpdatePayload(QString type, QVector<QString> nodes, QVector<int> costs)
{
    QJsonObject jsonObject;
    jsonObject["type"] = type;

    QJsonArray nodesArray;
    for (const QString& node : nodes)
            nodesArray.append(node);

    jsonObject["nodes"] = nodesArray;

    QJsonArray costsArray;
    for (const auto& cost : costs)
        costsArray.append(cost);
    jsonObject["costs"] = costsArray;

    QJsonDocument jsonDoc(jsonObject);
    return QString(jsonDoc.toJson(QJsonDocument::Compact));
}
```

This method extracts a JSON object from a JSON string. It parses the string into a QJsonDocument and returns the document's object. If the string is not a valid JSON, it logs a warning and returns an empty JSON object.
```cpp
QJsonObject RIP::extractUpdatePayloadJson(const QString &jsonString)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());

    if (!jsonDoc.isObject()) {
        qWarning() << "Invalid JSON string";
        return *(new QJsonObject);
    }

    return jsonDoc.object();
}
```

This method extracts a vector of node strings from a JSON object. It reads the "nodes" array from the JSON object and converts each value to a string, which it adds to the vector. It logs the extracted nodes for debugging purposes.
```cpp
QVector<QString> RIP::extractNodes(QJsonObject update)
{
    QVector<QString> nodes;
    QJsonArray nodesArray = update["nodes"].toArray();
    for (const QJsonValue& nodeValue : nodesArray) {
        nodes.append(nodeValue.toString());
    }
    qDebug() << "Nodes:" << nodes;
    return nodes;
}
```

This method extracts a vector of costs from a JSON object. It reads the "costs" array from the JSON object and converts each value to an integer, which it adds to the vector. It logs the extracted costs for debugging purposes.
```cpp
QVector<int> RIP::extractCosts(QJsonObject update)
{
    QVector<int> costs;
    QJsonArray costsArray = update["costs"].toArray();
    for (const QJsonValue& costValue : costsArray) {
        costs.append(costValue.toInt());
    }
    qDebug() << "Costs:" << costs;
    return costs;
}
```

This method handles HELLO packets. It extracts the source IP address from the packet and adds a route to the routing table with the source IP as both the destination and next hop. It then generates an update packet and sets m_updateIsReady to true.
```cpp
void RIP::handleHello(const PacketPtr_t &packet, const QSharedPointer<Port> &port)
{
    IpPtr_t nodeIP;
    if(packet->ipVersion() == UT::IPVersion::IPv4)
        nodeIP = IPv4_t::createIpPtr(packet->ipv4Header().sourceIp(), DEFAULT_MASK);
    else
        nodeIP = IPv6_t::createIpPtr(packet->ipv6Header().sourceIp(), DEFAULT_IPV6_PREFIX_LENGTH);
    m_routingTable->addRoute(nodeIP, nodeIP, port, PROTOCOL, NEIGHBOR_COST);
    generateUpdatePacket();
    m_updateIsReady = true;
}
```

This method handles UPDATE packets. It extracts the next hop IP address from the packet and updates the routing table with the new routes and costs. If any routes are updated, it sets m_updateIsReady to true. If no routes are updated for a certain number of times, it marks the routing process as finished and emits a signal. It then generates an update packet.
```cpp
void RIP::handleUpdate(const PacketPtr_t &packet, const QVector<QString> &nodes, const QVector<int> costs, const QSharedPointer<Port> &port)
{
    IpPtr_t nextHopIP;
    bool isUpdated = false;
    if(packet->ipVersion() == UT::IPVersion::IPv4)
        nextHopIP = IPv4_t::createIpPtr(packet->ipv4Header().sourceIp(), DEFAULT_MASK);
    else
        nextHopIP = IPv6_t::createIpPtr(packet->ipv6Header().sourceIp(), DEFAULT_IPV6_PREFIX_LENGTH);
    for (int i = 0; i<nodes.size(); i++){
        auto node = nodes[i];
        IpPtr_t nodeIP;
        if (node.contains(':'))
            nodeIP = IPv6_t::createIpPtr(node, DEFAULT_IPV6_PREFIX_LENGTH);
        else
            nodeIP = IPv4_t::createIpPtr(node, DEFAULT_MASK);
        int currentCost = m_routingTable->getRouteCost(nodeIP);
        int costFromNeighbor = costs[i] + NEIGHBOR_COST;
        if (currentCost > costFromNeighbor){
            m_routingTable->updateRoute(nodeIP ,nextHopIP, port, costFromNeighbor);
            isUpdated = true;
        }
    }
    if (!isUpdated)
        m_notUpdatedTimes++;
    else
        m_updateIsReady = true;
    if (m_notUpdatedTimes >= FINISH_THRESHOLD){
        m_isFinished = true;
        Q_EMIT routingFinished(*m_routingTable);
    }
    generateUpdatePacket();
}
```

This method generates an update packet. It creates a fake destination IP address and constructs a packet with the appropriate headers. It retrieves the nodes and costs from the routing table and stores the generated update payload in the packet's payload. It then sets the m_updatePacket member variable to the generated packet.
```cpp
QString RIP::generateUpdatePacket()
{
    IpPtr_t fakeDest = IPv4_t::createIpPtr("255.255.255.255", "255.255.255.255");
    QByteArray payload ;
    DataLinkHeader *dh = new DataLinkHeader(this->m_routerMacAddress, this->m_routerMacAddress);
    TCPHeader *th = new TCPHeader(BROADCAST_ON_ALL_PORTS, BROADCAST_ON_ALL_PORTS);
    Packet *update = new Packet(UT::PacketType::Control, UT::PacketControlType::RIP,
                                       1, 0, 0, fakeDest, payload, *dh, *th, m_routerIpv4Header, m_routerIpv6Header,
                                       RIP_TTL);
    auto nodes = m_routingTable->getNodes();
    auto costs = m_routingTable->getCosts();

    update->storeStringInPayload(generateUpdatePayload(UPDATE, nodes, costs));
    m_updatePacket = *update;
}
```

# OSPF

This is the constructor for the OSPF class. It initializes the OSPF object with the router's IP address (routerIp), MAC address (routerMac), and parent object (parent). It also initializes various member variables, including m_lsaPacket, m_routerIpv4Header, and m_routerIpv6Header. The constructor sets up the initial state of the OSPF object, including setting the source IP addresses for the IPv4 and IPv6 headers.
```cpp
OSPF::OSPF(IPv4Ptr_t routerIp, MacAddress routerMac, QObject* parent)
    : QObject(parent),
    m_routerIp(routerIp),
    m_lsaPacket(UT::PacketType::Control, UT::PacketControlType::OSPF, 1, 0, 0,
                   IPv4_t::createIpPtr("0.0.0.0", "255.255.255.0"), QByteArray(),
                   DataLinkHeader(MacAddress("00:00:00:00:00:00"), MacAddress("00:00:00:00:00:00")),
                   TCPHeader(0, 0), IPHv4_t(), IPHv6_t(), OSPF_TTL, this),
    m_routerMac(routerMac),
    m_lsaIsReady(false),
    m_isFinished(false),
    m_notUpdatedTimes(0) {
    m_routingTable = new RoutingTable(this);
    m_routerIpv4Header = *(new IPHv4_t());
    m_routerIpv4Header.setSourceIp(m_routerIp);
    m_routerIpv6Header = *(new IPHv6_t());
    m_routerIpv6Header.setSourceIp(m_routerIp->toIPv6());
}
```

This method starts the OSPF process. It creates a fake destination IP address and initializes a Packet object representing an OSPF hello packet. The hello packet's payload is generated using the generateLSAPayload method. The method sets the m_lsaPacket member variable to the hello packet and marks the LSA (Link State Advertisement) as ready by setting m_lsaIsReady to true.
```cpp
void OSPF::run()
{
    IpPtr_t fakeDest = IPv4_t::createIpPtr("255.255.255.255", "255.255.255.255");
    QByteArray payload;
    DataLinkHeader *dh = new DataLinkHeader(this->m_routerMac, this->m_routerMac);
    TCPHeader *th = new TCPHeader(BROADCAST_ON_ALL_PORTS, BROADCAST_ON_ALL_PORTS);
    Packet *hello = new Packet(UT::PacketType::Control, UT::PacketControlType::OSPF,
                                        1, 0, 0, fakeDest, payload, *dh, *th, m_routerIpv4Header, m_routerIpv6Header,
                                        OSPF_TTL);

    QVector<IpPtr_t> nodes = {};

    hello->storeStringInPayload(generateLSAPayload());
    m_lsaPacket = *hello;
    m_lsaIsReady = true;
}
```

This method processes received LSA packets. It reads the payload from the packet and parses it into a JSON object using the parsePayload method. The method then updates the topology graph with the parsed LSA data using the updateTopologyGraph method and computes the routing table.
```cpp
void OSPF::processLSAPacket(const PacketPtr_t &packet) {
    QString payload = packet->readStringFromPayload();
    QJsonObject lsaData = parsePayload(payload);
    updateTopologyGraph(lsaData);
    computeRoutingTable();
}
```

This method updates the topology graph with the received LSA data. It extracts the router's name and its links (neighbors) from the LSA data and adds the neighbors to the topology graph, ensuring no repeated neighbors are added.
```cpp
void OSPF::updateTopologyGraph(const QJsonObject &lsaData) {
    qDebug() << "Updating topology graph.";
    QString router = lsaData["router"].toString();
    QJsonArray links = lsaData["links"].toArray();

    for (const auto& link : links) {
        QString neighbor = link.toString();
        m_topologyGraph[router].insert(neighbor);//prevent repeted neighbors
    }
}
```

This method generates the payload for an OSPF hello packet. It creates a JSON object representing the hello packet, including the packet type (HELLO) and the router's IP address. The method returns the JSON object as a compact string.
```cpp
QString OSPF::generateHelloPayload() {
    QJsonObject helloPacket;
    helloPacket["type"] = HELLO;
    helloPacket["router"] = m_routerIp->toString();

    QJsonDocument doc(helloPacket);
    return QString(doc.toJson(QJsonDocument::Compact));
}
```

This method generates the payload for an OSPF LSA packet. It creates a JSON object representing the LSA packet, including the packet type (LSA), the router's IP address, and its links (neighbors). The method returns the JSON object as a compact string.
```cpp
QString OSPF::generateLSAPayload() {
    QJsonObject lsaPacket;
    lsaPacket["type"] = LSA;
    lsaPacket["router"] = m_routerIp->toString();

    QJsonArray links;
    for (const auto& neighbor : m_topologyGraph[m_routerIp->toString()]) {
        links.append(neighbor);
    }

    lsaPacket["links"] = links;

    QJsonDocument doc(lsaPacket);
    return QString(doc.toJson(QJsonDocument::Compact));
}
```

This method parses a JSON payload string into a JSON object. It takes the payload string and converts it into a QJsonDocument, then extracts and returns the JSON object from the document.
```cpp
QJsonObject OSPF::parsePayload(const QString& payload) {
    QJsonDocument doc = QJsonDocument::fromJson(payload.toUtf8());
    return doc.object();
}
```

This method checks if the LSA packet is ready. It returns the value of the m_lsaIsReady member variable, indicating whether the LSA packet has been prepared and is ready for use.
```cpp
bool OSPF::isLSAReady() {
    return m_lsaIsReady;
}
```
