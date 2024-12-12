#ifndef GLOBALS_H
#define GLOBALS_H

#include <QObject>
#include <vector>

// int MAX_BUFFER_SIZE = 20;

namespace UT
{

enum class IPVersion
{
    IPv4,
    IPv6
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

enum class DistributionType
{
    Poisson,
    Pareto
};

enum class TopologyType
{
    Mesh,
    RingStar,
    Torus
};

struct PacketDetails
{
    int senderID;
    int receiverID;
    PacketType packetType;

    PacketDetails(int sender, int receiver, PacketType type)
        : senderID(sender), receiverID(receiver), packetType(type) {}
};
}    // namespace UT

Q_DECLARE_METATYPE(UT::PacketDetails)
Q_DECLARE_METATYPE(std::vector<UT::PacketDetails>)

#endif    // GLOBALS_H
