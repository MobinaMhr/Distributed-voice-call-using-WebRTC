#include "IP.h"
#include <stdio.h>

AbstractIP::AbstractIP(QObject *parent) :
    QObject {parent}
{}

/**
 * ===========================================
 * ===========================================
 * ===========================================
 * @brief The IP class for IPv4.
 * ===========================================
 * ===========================================
 * ===========================================
 */

IP<UT::IPVersion::IPv4>::IP(QObject *parent) :
    AbstractIP(parent), m_ipValue(DEFAULT_IP_VALUE), m_subnetMask(DEFAULT_SUBNET_MASK_VALUE){} //TODO:check subnetMask value

IP<UT::IPVersion::IPv4>::IP(const QString &ipString, const QString &subnetMask, QObject *parent) :
    AbstractIP(parent), m_ipValue(DEFAULT_IP_VALUE), m_subnetMask(DEFAULT_SUBNET_MASK_VALUE){

    QStringList parts = ipString.split('.');
    if (parts.size() != 4)
        throw std::invalid_argument("Invalid IPv4 format");
    for (int i = 0; i < 4; ++i)
        m_ipValue |= (parts[i].toInt() & 0xFF) << (24 - 8 * i);

    if (!subnetMask.isEmpty())
        setSubnetMask(subnetMask);
    else
        setSubnetMask(DEFAULT_SUBNET_MASK);
}

IP<UT::IPVersion::IPv4>::IP(uint32_t ipValue, const QString &subnetMask, QObject *parent) :
    AbstractIP(parent), m_ipValue(ipValue), m_subnetMask(DEFAULT_SUBNET_MASK_VALUE){
    if (!subnetMask.isEmpty())
        setSubnetMask(subnetMask);
    else
        setSubnetMask(DEFAULT_SUBNET_MASK);
}

IP<UT::IPVersion::IPv4>::~IP() {};

QString IP<UT::IPVersion::IPv4>::toString() const
{
    QStringList parts;
    for (int i = 0; i < 4; ++i)
        parts << QString::number((m_ipValue >> (24 - 8 * i)) & 0xFF);
    return parts.join('.');
}

/**
 * ===========================================
 * ===========================================
 * ===========================================
 * @brief The IP class for IPv6.
 * ===========================================
 * ===========================================
 * ===========================================
 */

IP<UT::IPVersion::IPv6>::IP(QObject *parent) :
    AbstractIP(parent)
{
    m_ipValue = std::numeric_limits<uint64_t>::max();
}

IP<UT::IPVersion::IPv6>::IP(const QString &ipString, QObject *parent) :
    AbstractIP(parent)
{
    m_ipValue = std::numeric_limits<uint64_t>::max();
    fromString(ipString);
    toValueImpl();
}

IP<UT::IPVersion::IPv6>::IP(uint64_t ipValue, QObject *parent) :
    AbstractIP(parent)
{
    m_ipValue = ipValue;
    fromIPValue(ipValue);
    toStringImpl();
}

IP<UT::IPVersion::IPv6>::~IP() {};
