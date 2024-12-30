#ifndef GLOBALS_H
#define GLOBALS_H

#include <QObject>
#include <vector>

namespace UT
{

enum class NodeState
{
    Alive,
    Dead
};

enum class IPVersion
{
    IPv4,
    IPv6
};

enum class RoutingMode
{
    Routing,
    Simulating
};

enum class PortState
{
    Reserved,
    Idle,
    Sending
};

enum class PacketType
{
    Data,
    Control
};

enum class PacketControlType
{
    Request,
    Response,
    Acknowledge,
    Error,
    DHCPDiscovery,
    DHCPOffer,
    DHCPRequest,
    DHCPAcknowledge,
    DHCPNak,
    RIP,
    OSPF,
};

// const std::map<PacketControlType, QString> packetControlTypeStrings = {
//  {PacketControlType::Request, "Request"},
//  {PacketControlType::Response, "Response"},
//  {PacketControlType::Acknowledge, "Acknowledge"},
//  {PacketControlType::Error, "Error"},
//  {PacketControlType::DHCPDiscovery, "DHCPDiscovery"},
//  {PacketControlType::DHCPOffer, "DHCPOffer"},
//  {PacketControlType::DHCPRequest, "DHCPRequest"},
//  {PacketControlType::DHCPAcknowledge, "DHCPAcknowledge"},
//  {PacketControlType::DHCPNak, "DHCPNak"},
//  {PacketControlType::RIP, "RIP"},
//  {PacketControlType::OSPF, "OSPF"},
//  };

// QString toString(PacketControlType type) {
//     auto it = packetControlTypeStrings.find(type);
//     if (it != packetControlTypeStrings.end()) {
//         return it->second;
//     }
//     return "Unknown";
// }

enum class DistributionType
{
    Poisson,
    Pareto
};

enum class TopologyType
{
    Mesh,
    RingStar,
    Torus,
    None
};

struct PacketDetails
{
    int senderID;
    int receiverID;
    PacketType packetType;
    int cycle;

    PacketDetails(int sender, int receiver, PacketType type, int _cycle)
        : senderID(sender), receiverID(receiver), packetType(type), cycle(_cycle) {}
};
}    // namespace UT

Q_DECLARE_METATYPE(UT::PacketDetails)
Q_DECLARE_METATYPE(std::vector<UT::PacketDetails>)

#endif    // GLOBALS_H
