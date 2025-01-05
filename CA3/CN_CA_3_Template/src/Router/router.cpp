#include "Router.h"
#include "qdebug.h"

const QString LOG_TITLE = "Router:: ";
const int MAX_BUFFER_SIZE = 20;

Router::Router(int id, const MacAddress &macAddress, int portCount, UT::IPVersion ipVersion, int bufferSize, QThread *parent)
    : Node(id, macAddress, portCount, ipVersion, parent),
    m_routing_table(new RoutingTable),
    m_ports(portCount),
    m_bufferSize(bufferSize),
    m_dhcp(nullptr),
    m_isRouting(false),
    m_isBgpWorking(false),
    m_rip((new RIP(IPv4Ptr_t::create((new IPv4_t("255.255.255.255", DEFAULT_MASK, this))),
                   MacAddress(DEFAULT_MAC)))),
    m_ospf(new OSPF(IPv4Ptr_t::create((new IPv4_t("255.255.255.255", DEFAULT_MASK, this))), m_macAddress, this)),
    m_bgpPort(ROUTER_IS_NOT_GATEWAY){
        for (int i = 0; i < portCount; ++i) {
        m_ports[i] = QSharedPointer<Port>::create(i);

        connect(this, &Router::sendPacket, m_ports[i].data(), &Port::sendPacket, Qt::AutoConnection);
        connect(m_ports[i].data(), &Port::packetReceived, this, &Router::receivePacket, Qt::AutoConnection);
    }
    QString ipv = (ipVersion == UT::IPVersion::IPv4) ? "IPV4" : "IPV6";
    log("Router " + QString::number(id) + "by ipversion: " + ipv + "by macAddress: " + macAddress.toString() +
        "created successfully");
}

Router::~Router() {}

void Router::addRoutingEntry(IpPtr_t &destinationIp, IpPtr_t &nextHopIp, QSharedPointer<Port> &port, int metric) {
    m_routing_table->addRoute(destinationIp, nextHopIp, port, "TODO::", metric);
}

bool Router::isBufferAtCapacity() {
    return m_buffer.size() >= MAX_BUFFER_SIZE;
}

int Router::findBufferPositionForPacket(UT::PacketType packetType) {
    if (packetType == UT::PacketType::Data) {
        if (!isBufferAtCapacity()) {
            return static_cast<int>(m_buffer.size());
        }
    } else if (packetType == UT::PacketType::Control) {
        auto it = std::find_if(m_buffer.rbegin(), m_buffer.rend(), [](const PacketPtr_t &pkt) {
            return pkt->packetType() == UT::PacketType::Control;
        });

        /// Potential Bugs. TODO:: fix the findBuff for Control Packets.

        if (it != m_buffer.rend()) {
            if (isBufferAtCapacity()) m_buffer.pop_back();
            return static_cast<int>(it.base() - m_buffer.begin());
        } else if (!isBufferAtCapacity()) {
            return -1;
        }
    }

    return -1;
}

void Router::handleDhcpDiscovery(PacketPtr_t packet)
{
    if (m_dhcp != nullptr){
        int id = packet->readIntFromPayload();
        QString sugestedIp = m_dhcp->assignIPToNode(id);
        IpPtr_t fakeDest = IPv4_t::createIpPtr("255.255.255.255", "255.255.255.255");
        QByteArray payload ;
        DataLinkHeader *dh = new DataLinkHeader(this->m_macAddress, packet->dataLinkHeader().destination());

        TCPHeader *th = new TCPHeader(BROADCAST_ON_ALL_PORTS, BROADCAST_ON_ALL_PORTS);
        IPHv4_t *iphv4 = new IPHv4_t();
        IPHv6_t *iphv6 = new IPHv6_t();
        Packet *offer = new Packet(
            UT::PacketType::Control, UT::PacketControlType::DHCPOffer, 1, 0, 0,
            fakeDest, payload, *dh, *th, *iphv4, *iphv6, DHCP_TTL
        );
        offer->storeStringInPayload(sugestedIp);
        PacketPtr_t offerPt = PacketPtr_t(offer);
        /// BUFFER HERE instead of send
        log("offer packet : " + offerPt->getLogMessage() + "\npayload:\n" + sugestedIp + "\ngenerated");
        // Q_EMIT sendPacket(offerPt, BROADCAST_ON_ALL_PORTS);
        // generate offer packet
    }
    else{
        Q_EMIT sendPacket(packet, BROADCAST_ON_ALL_PORTS);//should bufer it and send by clock tick
        log("discovery packet : " + packet->getLogMessage() + "\npayload:\n" +
            QString::number(packet->readIntFromPayload()) + "\nforwaeded");
    }
}

void Router::handleDhcpOffer(PacketPtr_t packet)
{
    if (packet->dataLinkHeader().source().toString() == m_macAddress.toString()){
        if (m_ipv4Address->toValue() == DEFAULT_IP_VALUE){
            QString sugestedIp = packet->readStringFromPayload();
            IpPtr_t fakeDest = IPv4_t::createIpPtr("255.255.255.255", "255.255.255.255");
            QByteArray payload ;
            DataLinkHeader *dh = new DataLinkHeader(this->m_macAddress,
                                                       packet->dataLinkHeader().destination());
            TCPHeader *th = new TCPHeader(BROADCAST_ON_ALL_PORTS, BROADCAST_ON_ALL_PORTS);
            IPHv4_t *iphv4 = new IPHv4_t();
            IPHv6_t *iphv6 = new IPHv6_t();
            Packet *req = new Packet(
                UT::PacketType::Control, UT::PacketControlType::DHCPRequest, 1, 0, 0,
                fakeDest, payload, *dh, *th, *iphv4, *iphv6, DHCP_TTL
            );
            req->storeIntInPayload(m_id);
            PacketPtr_t reqPt = PacketPtr_t(req);
            Q_EMIT sendPacket(reqPt, BROADCAST_ON_ALL_PORTS);
            log("request packet : " + reqPt->getLogMessage() + "\npayload:\n" +
                QString::number(m_id)  + "\ngenerated");
        }
    }
    else{
        Q_EMIT sendPacket(packet, BROADCAST_ON_ALL_PORTS);
        log("offer packet : " + packet->getLogMessage() + "\npayload:\n" +
            packet->readStringFromPayload() + "\nforwarded");
    }
}

void Router::handleDhcpReq(PacketPtr_t packet)
{
    if (m_dhcp != nullptr){
        QString response = createDhcpAckBody(packet);
        IpPtr_t fakeDest = IPv4_t::createIpPtr("255.255.255.255", "255.255.255.255");
        QByteArray payload ;
        DataLinkHeader *dh = new DataLinkHeader(this->m_macAddress,
                                                   packet->dataLinkHeader().destination());
        TCPHeader *th = new TCPHeader(BROADCAST_ON_ALL_PORTS, BROADCAST_ON_ALL_PORTS);
        IPHv4_t *iphv4 = new IPHv4_t();
        IPHv6_t *iphv6 = new IPHv6_t();
        Packet *ack = new Packet(
            UT::PacketType::Control, UT::PacketControlType::DHCPAcknowledge, 1, 0, 0,
            fakeDest, payload, *dh, *th, *iphv4, *iphv6, DHCP_TTL
        );
        ack->storeStringInPayload(response);
        PacketPtr_t ackPt = PacketPtr_t(ack);
        Q_EMIT sendPacket(ackPt, BROADCAST_ON_ALL_PORTS);
        log("ack packet : " + ackPt->getLogMessage() + "\npayload:\n" + response + "\ngenerated");
        // generate offer packet
    }
    else{
        Q_EMIT sendPacket(packet, BROADCAST_ON_ALL_PORTS);
        log("request packet : " + packet->getLogMessage() + "\npayload:\n" +
            QString::number(packet->readIntFromPayload()) + "\nforwarded");
    }
}

void Router::handleDhcpAck(PacketPtr_t packet)
{
    QString payload = packet->readStringFromPayload();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(payload.toUtf8());
    QString ip = jsonDoc.object().value("ip").toString();
    QString mask = jsonDoc.object().value("mask").toString();
    log("ack packet : " + packet->getLogMessage() + "\npayload:\n" +
        packet->readStringFromPayload() + "\nrecieved");
    setIP(ip, mask);
}

void Router::receivePacket(const PacketPtr_t &packet, uint8_t portNumber) {
    if (!packet) {
        qDebug() << name() << ": Received a null packet.";
        return;
    }
    packet->decreasePacketTtl();

    if (m_hasIP && !m_isRouting){
        if (!isPacketMine(packet) && !packet->shouldDrop()) {
            bufferPacket(packet);
            log("packet : \n" + packet->getLogMessage() + "\nbuffered");
            return;
        }//TODO:should move somewhere else;
        else if (packet->shouldDrop()){
            log("packet : \n" + packet->getLogMessage() + "\ndroped by the end of its ttl");
            return;
        }
    }///?????????

    switch (packet->packetType()) {
        case UT::PacketType::Control:
            processControlPacket(packet, portNumber);
            break;

        case UT::PacketType::Data:
            processDataPacket(packet);
            break;

        default:
            break;
    }
}

void Router::finishRouting(RoutingTable routingTable)
{
    m_routing_table = new RoutingTable(routingTable);
    log("routing finished : " + m_routing_table->getRoutingTableAsString());
    m_isRouting = false;
    Q_EMIT routingFinished(m_id);
}

void Router::getIP()
{
    if (m_dhcp != nullptr){
        QString sugestedIp = m_dhcp->assignIPToNode(m_id);
        setIP(sugestedIp, DEFAULT_MASK);
    }else
        sendDiscovery();
}

void Router::handleTick(int cycleNumebt)
{
    if (m_isRouting){
        handleRoutingPacket();
    }
}

QString Router::createDhcpAckBody(PacketPtr_t packet)
{
    int id = packet->readIntFromPayload();
    QString sugestedIp = m_dhcp->assignIPToNode(id);
    QJsonObject jsonObject;
    jsonObject["ip"] = sugestedIp;
    jsonObject["mask"] = DEFAULT_MASK;

    // Convert the JSON object to a QJsonDocument
    QJsonDocument jsonDoc(jsonObject);

    // Convert the JSON document to a QString
    QString jsonString = jsonDoc.toJson(QJsonDocument::Compact);

    return jsonString;
}

void Router::handleRoutingPacket()
{
    if (m_protocol == UT::PacketControlType::RIP){
        if (m_rip->isUpdateReady()){
            Packet *update = new Packet(m_rip->getUpdatePacket());
            Q_EMIT sendPacket(PacketPtr_t(update), BROADCAST_ON_ALL_PORTS);
        }
    }
    else if(m_protocol == UT::PacketControlType::OSPF){
        for (int i = 0 ; i < m_buffer.size(); i++){
            Q_EMIT sendPacket(m_buffer[i], BROADCAST_ON_ALL_PORTS);
        }
        m_buffer.clear();
        if (m_ospf->isLSAReady()){
            Packet *lsa = new Packet(m_ospf->getlsaPacket());
            Q_EMIT sendPacket(PacketPtr_t(lsa), BROADCAST_ON_ALL_PORTS);
        }
    }
}

void Router::handleOspfPacket(PacketPtr_t packet, int portNumber)
{
    QString payload = packet->readStringFromPayload();
    log("ospf packet : " + packet->getLogMessage() + "\npayload:\n" + payload + "\nrecevied on port : " +
        QString::number(portNumber));
    QJsonDocument doc = QJsonDocument::fromJson(payload.toUtf8());
    QString type = doc.object()["type"].toString();
    if (type != HELLO)
        bufferPacket(packet);
    m_ospf->handleOSPFPacket(packet, m_ports[portNumber]);
}

void Router::handleRipPacket(PacketPtr_t packet, int portNumber)
{
    QString payload = packet->readStringFromPayload();
    log("ospf packet : " + packet->getLogMessage() + "\npayload:\n" + payload + "\nrecevied on port : " +
        QString::number(portNumber));
    m_rip->handleRIPPacket(packet, m_ports[portNumber]);
}

void Router::bufferPacket(const PacketPtr_t &packet) {
    int position = findBufferPositionForPacket(packet->packetType());

    if (position == -1) {
        log("Packet Dropped: Buffer Full. Sequence:" + QString::number(packet->sequenceNumber()));
        return;
    }

    if (position == static_cast<int>(m_buffer.size())) {
        m_buffer.push_back(std::move(packet));
    } else {
        m_buffer.insert(m_buffer.begin() + position, std::move(packet));
    }
    QString packetType = (packet->packetType() == UT::PacketType::Data ? "Data Packet Added." : "Control Packet Added.");
    log(packetType + "Buffer Size:" + QString::number(m_buffer.size()));
}

void Router::routePackets() {
    for (auto it = m_buffer.begin(); it != m_buffer.end(); /* no increment here */) {
        const auto& packetPtr = *it;

        if (!packetPtr) {
            log( "Encountered a null packet pointer.");
            it = m_buffer.erase(it);
            continue;
        }

        log("Processing packet with sequence number:" + QString::number(packetPtr->sequenceNumber()));

        if (!m_routing_table->routeExists(packetPtr->destinationIP())) {
            log("No Route Exists for the packet");
            continue;
        }

        QSharedPointer<Port> hostPort = m_routing_table->getPort(packetPtr->destinationIP());
        int portIndex = hostPort->number();

        if (m_ports[portIndex]->state() == UT::PortState::Reserved || !m_hasIP || m_isRouting || m_isBgpWorking) {
            m_ports[portIndex]->setState(UT::PortState::Sending);
            log("Routing packet :\n" + packetPtr->getLogMessage() + "\nto port" + QString::number(portIndex));

            Q_EMIT sendPacket(packetPtr, hostPort->number());
            it = m_buffer.erase(it);
        } else if (m_ports[portIndex]->state() == UT::PortState::Sending) {
            log("Port" + QString::number(portIndex) + "is already sending. Keeping packet in buffer.");
            ++it;
        } else {
            log("Port" + QString::number(portIndex) + "is not bined and is idle.");
            ++it;
        }

        bool allPortsBusy = std::all_of(
          m_ports.begin(),
          m_ports.end(),
          [](PortPtr_t port) { return (port->state() == UT::PortState::Sending) ||
                                      (port->state() == UT::PortState::Idle); });

        if (allPortsBusy) {
            log("All ports are Busy. Stopping packet routing.");
            break;
        }
    }
}

PortPtr_t Router::getIdlePort() {
    for (auto port : m_ports) {
        if (port->state() == UT::PortState::Idle)
            return port;
    }
    return nullptr;
}

std::vector<PortPtr_t> Router::getPorts() {
    return m_ports;
}

void Router::setDhcp(int asNumber)
{
    this->m_dhcp = new DHCP(asNumber);
    log("I'm a DHCP");
}

void Router::route(UT::PacketControlType protocol)
{
    QString protocolName = (protocol == UT::PacketControlType::OSPF) ? "OSPF" : "RIP";
    log("start routing by protocol " + protocolName);
    m_isRouting = true;
    m_protocol = protocol;
    if (m_protocol == UT::PacketControlType::RIP){
        m_rip = new RIP(m_ipv4Address, m_macAddress, this);
        connect(m_rip, &RIP::routingFinished, this, &Router::finishRouting, Qt::AutoConnection);
        m_rip->run();
    }else if(m_protocol == UT::PacketControlType::OSPF){
        m_ospf = new OSPF(m_ipv4Address, m_macAddress, this);
        connect(m_ospf, &OSPF::routingFinished, this, &Router::finishRouting, Qt::AutoConnection);
        m_ospf->run();
    }
}

std::vector<QSharedPointer<Node>> Router::neighbors() {
    return m_neighbors;
}

void Router::processControlPacket(const PacketPtr_t &packet, uint8_t portNumber) {
    qDebug() << name() << ": Implement control packet handling logic.";
    if (m_ipv4Address->isInSubnet(packet->ipv4Header().sourceIp())){
        switch (packet->controlType()) {
            case UT::PacketControlType::DHCPDiscovery:
                handleDhcpDiscovery(packet);
                break;

            case UT::PacketControlType::DHCPOffer:
                handleDhcpOffer(packet);
                break;

            case UT::PacketControlType::DHCPRequest:
                handleDhcpReq(packet);
                break;

            case UT::PacketControlType::DHCPAcknowledge:
                handleDhcpAck(packet);
                break;
            case UT::PacketControlType::RIP:
                handleRipPacket(packet, portNumber);
                break;
            case UT::PacketControlType::OSPF:
                handleOspfPacket(packet, portNumber);
                break;
            case UT::PacketControlType::EBGP:
                handleEBgpPacket(packet);
                break;
            case UT::PacketControlType::IBGP:
                handleIBgp(packet, portNumber);
                break;

            default:
                break;
        }
    }
    // Add specific logic for Control Packet processing (e.g., routing updates, acknowledgments).
}

void Router::processDataPacket(const PacketPtr_t &packet) {
    qDebug() << name() << ": Implement data packet handling logic.";
    // Add specific logic for Data Packet handling, such as forwarding or delivering.
}

void Router::runBgp()
{
    if(m_bgpPort != ROUTER_IS_NOT_GATEWAY){
        IPHv4_t v4 = *(new IPHv4_t(this));
        v4.setSourceIp(m_ipv4Address);
        IPHv6_t v6 = *(new IPHv6_t(this));
        v6.setSourceIp(m_ipv4Address->toIPv6());
        IpPtr_t fakeDest = IPv4_t::createIpPtr("255.255.255.255", "255.255.255.255");
        QByteArray payload ;
        DataLinkHeader *dh = new DataLinkHeader(this->m_macAddress, this->m_macAddress);
        TCPHeader *th = new TCPHeader(m_bgpPort, BROADCAST_ON_ALL_PORTS);
        Packet *update = new Packet(
            UT::PacketType::Control, UT::PacketControlType::EBGP, 1, 0, 0,
            fakeDest, payload, *dh, *th, v4, v6, RIP_TTL
        );

        update->storeStringInPayload(generateEBgpPayload());
        //store it to send on bgpProt!!!
    }
    /// should send the Ebgp packet
    /// shoul send the Ibgp packet on its own routing table
}

void Router::setBgpPort(int portnumber)
{
    m_bgpPort = portnumber;
}

QString Router::generateEBgpPayload()
{
    QMap<IpPtr_t, RoutingTable::RouteEntry> routes = m_routing_table->getAllRoutes();
    QJsonObject jsonObject;
    jsonObject["type"] = EBGP_PROTOCOL;

    QJsonArray nodesArray;
    QJsonArray costsArray;
    for (const IpPtr_t& node : routes.keys()){
        nodesArray.append(node->toIPv4()->toString());
        costsArray.append(routes[node].metric);
    }
    jsonObject["nodes"] = nodesArray;
    jsonObject["costs"] = costsArray;
    QJsonDocument jsonDoc(jsonObject);
    return QString(jsonDoc.toJson(QJsonDocument::Compact));
}

void Router::handleEBgpPacket(const PacketPtr_t &packet)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(packet->readStringFromPayload().toUtf8());
    QJsonObject inputPayload = jsonDoc.object();
    inputPayload["type"] = IBGP_PROTOCOL;
    QJsonDocument newJsonDoc(inputPayload);
    QString newPayload = QString(jsonDoc.toJson(QJsonDocument::Compact));
    QMap<IpPtr_t, RoutingTable::RouteEntry> currentRoutingTable = m_routing_table->getAllRoutes();
    for (const IpPtr_t& node : currentRoutingTable.keys()) {
        Packet ibgpPacket = generateIBgpPacket(node, newPayload);
        // send the packet;
    }
    IpPtr_t ip =  (m_ipVersion == UT::IPVersion::IPv4) ? IpPtr_t(m_ipv4Address) : IpPtr_t(m_ipv6Address);
    updateRoutingTable(newJsonDoc.object(), IPv4_t::createIpPtr(packet->ipv4Header().sourceIp(), DEFAULT_MASK),
                       m_ports[m_bgpPort]);
}

Packet Router::generateIBgpPacket(const IpPtr_t &dest, QString payload)
{
    IPHv4_t v4 = *(new IPHv4_t(this));
    v4.setSourceIp(m_ipv4Address);
    IPHv6_t v6 = *(new IPHv6_t(this));
    v6.setSourceIp(m_ipv4Address->toIPv6());
    QByteArray _payload ;
    DataLinkHeader *dh = new DataLinkHeader(this->m_macAddress, this->m_macAddress);
    TCPHeader *th = new TCPHeader(m_bgpPort, BROADCAST_ON_ALL_PORTS);
    Packet *update = new Packet(
        UT::PacketType::Control, UT::PacketControlType::IBGP, 1, 0, 0,
        dest, _payload, *dh, *th, v4, v6, RIP_TTL
    );

    update->storeStringInPayload(payload);;
    return *update;
}

void Router::updateRoutingTable(QJsonObject ibgp, IpPtr_t sourceIp, PortPtr_t port)
{
    QVector<QString> nodes;
    QVector<int> costs;
    QJsonArray nodesArray = ibgp["nodes"].toArray();
    QJsonArray costsArray = ibgp["costs"].toArray();

    for (const QJsonValue& nodeValue : nodesArray)
        nodes.append(nodeValue.toString());
    for (const QJsonValue& costValue : costsArray)
        costs.append(costValue.toInt());

    IpPtr_t nextHopIP = m_routing_table->getNextHop(sourceIp);
    if (nextHopIP == nullptr) {
        nextHopIP = sourceIp;
    }
    for (int i = 0; i<nodes.size(); i++){
        auto node = nodes[i];
        IpPtr_t nodeIP;
        nodeIP = IPv4_t::createIpPtr(node, DEFAULT_MASK);
        int currentCost = m_routing_table->getRouteCost(nodeIP);
        int costToGateway = m_routing_table->getRouteCost(sourceIp);
        if (costToGateway == std::numeric_limits<int>::max()) {
            costToGateway = 0;
        }
        int costFromNeighbor = costs[i] + NEIGHBOR_COST + costToGateway;
        if (currentCost > costFromNeighbor) {
            m_routing_table->updateRoute(nodeIP ,nextHopIP, port, costFromNeighbor);
        }

    }
}

void Router::handleIBgp(const PacketPtr_t &packet, const int &portnumber)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(packet->readStringFromPayload().toUtf8());
    IpPtr_t ip =  (m_ipVersion == UT::IPVersion::IPv4) ? IpPtr_t(m_ipv4Address) : IpPtr_t(m_ipv6Address);
    updateRoutingTable(jsonDoc.object(), IPv4_t::createIpPtr(packet->ipv4Header().sourceIp(), DEFAULT_MASK),
                       m_ports[portnumber]);
}
