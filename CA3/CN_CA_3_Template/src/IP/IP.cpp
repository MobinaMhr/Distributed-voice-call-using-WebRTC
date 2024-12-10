// #include "IP.h"

// AbstractIP::AbstractIP(QObject *parent) :
//     QObject {parent}
// {}

// /**
//  * ===========================================
//  * ===========================================
//  * ===========================================
//  * @brief The IP class for IPv4.
//  * ===========================================
//  * ===========================================
//  * ===========================================
//  */

// IP<UT::IPVersion::IPv4>::IP(QObject *parent) :
//     AbstractIP(parent)
// {
//     m_ipValue = std::numeric_limits<uint64_t>::max();
// }

// IP<UT::IPVersion::IPv4>::IP(const QString &ipString, QObject *parent) :
//     AbstractIP(parent)
// {
//     m_ipValue = std::numeric_limits<uint64_t>::max();
// }

// IP<UT::IPVersion::IPv4>::IP(uint64_t ipValue, QObject *parent) :
//     AbstractIP(parent)
// {
//     m_ipValue = ipValue;
// }

// IP<UT::IPVersion::IPv4>::~IP() {};

// /**
//  * ===========================================
//  * ===========================================
//  * ===========================================
//  * @brief The IP class for IPv6.
//  * ===========================================
//  * ===========================================
//  * ===========================================
//  */

// IP<UT::IPVersion::IPv6>::IP(QObject *parent) :
//     AbstractIP(parent)
// {
//     m_ipValue = std::numeric_limits<uint64_t>::max();
// }

// IP<UT::IPVersion::IPv6>::IP(const QString &ipString, QObject *parent) :
//     AbstractIP(parent)
// {
//     m_ipValue = std::numeric_limits<uint64_t>::max();
//     fromString(ipString);
//     toValueImpl();
// }

// IP<UT::IPVersion::IPv6>::IP(uint64_t ipValue, QObject *parent) :
//     AbstractIP(parent)
// {
//     m_ipValue = ipValue;
//     fromIPValue(ipValue);
//     toStringImpl();
// }

// IP<UT::IPVersion::IPv6>::~IP() {};
