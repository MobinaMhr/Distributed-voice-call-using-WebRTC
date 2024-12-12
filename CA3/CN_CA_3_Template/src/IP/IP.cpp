#include "IP.h"
#include <stdio.h>

namespace {
uint32_t subnetMaskToValue(const QString &subnetMask) {
    QStringList parts = subnetMask.split('.');
    if (parts.size() != 4)
        throw std::invalid_argument("Invalid subnet mask format");
    uint32_t value = 0;
    for (int i = 0; i < 4; ++i)
        value |= (parts[i].toInt() & 0xFF) << (24 - 8 * i);
    return value;
    }

QString valueToSubnetMask(uint32_t value) {
    QStringList parts;
    for (int i = 0; i < 4; ++i)
        parts << QString::number((value >> (24 - 8 * i)) & 0xFF);
    return parts.join('.');
    }
}

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

QString IP<UT::IPVersion::IPv4>::toString() const{
    QStringList parts;
    for (int i = 0; i < 4; ++i)
        parts << QString::number((m_ipValue >> (24 - 8 * i)) & 0xFF);
    return parts.join('.');
}

uint32_t IP<UT::IPVersion::IPv4>::toValue() const{
    return m_ipValue;
}


QString IP<UT::IPVersion::IPv4>::getSubnetMask() const
{
    return valueToSubnetMask(m_subnetMask);
}

void IP<UT::IPVersion::IPv4>::setSubnetMask(const QString &subnetMask)
{
    m_subnetMask = subnetMaskToValue(subnetMask);
}

IPv4Ptr_t IP<UT::IPVersion::IPv4>::getGateway() const
{
    if (m_gateway) {
        return m_gateway;//may cause bug!!
    } else {
        throw std::logic_error("Gateway is not set");
    }
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


