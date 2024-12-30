#include "IP.h"
#include <stdio.h>
#include <stdexcept>
#include <QStringList>

namespace {
uint32_t subnetMaskToValue(const QString &subnetMask) {
    QStringList parts = subnetMask.split('.');
    if (parts.size() != 4)
        throw std::invalid_argument(SUBNET_MASK_FORMAT_ERROR);
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

QByteArray bitwiseAnd(const QByteArray &array1, const QByteArray &array2) {
    if (array1.size() != array2.size()) {
        qWarning() << "Byte arrays must have the same size for bitwise operations.";
        return QByteArray();
    }
    QByteArray result(array1.size(), 0);
    for (int i = 0; i < array1.size(); ++i) {
        result[i] = array1[i] & array2[i];
    }
    return result;
    }
}

AbstractIP::AbstractIP(QObject *parent) :
    QObject {parent}
{}

// bool AbstractIP::operator<(const AbstractIP& other) const {
//     IPv4Ptr_t myIPV4 = this->toIPv4();
//     IPv4Ptr_t theirIPV4 = other.toIPv4();
//     return myIPV4->toValue() < theirIPV4->toValue();
// }

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
        throw std::logic_error(EMPTY_GATEWAY_ERROR);
    }
}

void IP<UT::IPVersion::IPv4>::setGateway(IPv4Ptr_t &gateway)
{
    m_gateway = gateway;
}

QPair<QString, QString> IP<UT::IPVersion::IPv4>::getSubnetRange() const
{
    uint32_t network = m_ipValue & m_subnetMask;
    uint32_t broadcast = network | ~m_subnetMask;
    return { QHostAddress(network).toString(), QHostAddress(broadcast).toString() };
}

bool IP<UT::IPVersion::IPv4>::isInSubnet(const QString &otherIP) const
{
    QHostAddress addr(otherIP);
    if (addr.protocol() != QAbstractSocket::IPv4Protocol) {
        return false;
    }
    uint32_t otherValue = addr.toIPv4Address();
    return (m_ipValue & m_subnetMask) == (otherValue & m_subnetMask);
}

IPv4Ptr_t IP<UT::IPVersion::IPv4>::toIPv4() const
{
    return QSharedPointer<IPv4_t>::create(new IPv4_t(m_ipValue, getSubnetMask()));
}

IPv6Ptr_t IP<UT::IPVersion::IPv4>::toIPv6() const
{
    QByteArray ipv6Bytes(IPV6_Length_IN_BYTES, 0);
    ipv6Bytes[10] = static_cast<char>(0xFF);
    ipv6Bytes[11] = static_cast<char>(0xFF);
    ipv6Bytes[12] = static_cast<char>((m_ipValue >> 24) & 0xFF);
    ipv6Bytes[13] = static_cast<char>((m_ipValue >> 16) & 0xFF);
    ipv6Bytes[14] = static_cast<char>((m_ipValue >> 8) & 0xFF);
    ipv6Bytes[15] = static_cast<char>(m_ipValue & 0xFF);
    // Create and return an IPv6 instance
    return QSharedPointer<IPv6_t>::create(new IPv6_t(ipv6Bytes, IPV6_Length_IN_BITS));
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
    AbstractIP(parent), m_ipValue(IPV6_Length_IN_BYTES, IPV6_DEFAULT_FILL_VALUE),
    m_prefixLength(DEFAULT_IPV6_PREFIX_LENGTH){}

IP<UT::IPVersion::IPv6>::~IP() {};

IP<UT::IPVersion::IPv6>::IP(const QString &ipString, int prefixLength, QObject *parent) :
    AbstractIP(parent), m_ipValue(IPV6_Length_IN_BYTES, IPV6_DEFAULT_FILL_VALUE),
    m_prefixLength(prefixLength){
    QString modifiedString = ipString;
    modifiedString.remove(IPV6_DELIM);
    m_ipValue = QByteArray::fromHex(modifiedString.toUtf8());
    if (m_ipValue.size() != IPV6_Length_IN_BYTES)
        throw std::invalid_argument(IPV6_VALUE_ERROR);
}

IP<UT::IPVersion::IPv6>::IP(const QByteArray &ipValue, int prefixLength, QObject *parent) :
    AbstractIP(parent), m_ipValue(ipValue), m_prefixLength(prefixLength){
    if (m_ipValue.size() != IPV6_Length_IN_BYTES)
        throw std::invalid_argument(IPV6_VALUE_ERROR);
}

QString IP<UT::IPVersion::IPv6>::toString() const{
    QString result;
    for (int i = 0; i < m_ipValue.size(); i += 2) {
        if (i > 0) result += ':';
        result += QString::number((m_ipValue[i] << 8) | m_ipValue[i + 1], 16);
    }
    return result;
}

QByteArray IP<UT::IPVersion::IPv6>::toValue() const
{
    return m_ipValue;
}

int IP<UT::IPVersion::IPv6>::getPrefixLength() const
{
    return m_prefixLength;
}

void IP<UT::IPVersion::IPv6>::setPrefixLength(int prefixLength)
{
    if (prefixLength < 0 || prefixLength > IPV6_Length_IN_BITS)
        throw std::invalid_argument(INVALID_PREFIX_LENGTH_ERROR);
    m_prefixLength = prefixLength;
}

QPair<QString, QString> IP<UT::IPVersion::IPv6>::getSubnetRange() const
{
    QByteArray lowerBound = m_ipValue;
    QByteArray upperBound = m_ipValue;
    int hostBits = IPV6_Length_IN_BITS - m_prefixLength;

    for (int i = 15; i >= 0 && hostBits > 0; --i) {
        int clearBits = std::min(8, hostBits);
        upperBound[i] |= (0xFF >> (8 - clearBits));
        hostBits -= clearBits;
    }
    return { QHostAddress(lowerBound).toString(), QHostAddress(upperBound).toString() };
}

bool IP<UT::IPVersion::IPv6>::isInSubnet(const QString &otherIP) const
{
    QHostAddress addr(otherIP);
    if (addr.protocol() != QAbstractSocket::IPv6Protocol) {
        return false;
    }
    QString modifiedString = otherIP;
    modifiedString.remove(IPV6_DELIM);
    QByteArray otherValue = QByteArray::fromHex(modifiedString.toUtf8());
    QByteArray mask(IPV6_Length_IN_BYTES, IPV6_DEFAULT_FILL_VALUE);
    int hostBits = IPV6_Length_IN_BITS - m_prefixLength;

    for (int i = 15; i >= 0 && hostBits > 0; --i) {
        int clearBits = std::min(8, hostBits);
        mask[i] = ~((1 << clearBits) - 1);
        hostBits -= clearBits;
    }
    return bitwiseAnd(m_ipValue, mask) == bitwiseAnd(otherValue, mask);
}

IPv6Ptr_t IP<UT::IPVersion::IPv6>::toIPv6() const {
    return QSharedPointer<IPv6_t>::create(new IPv6_t(m_ipValue, m_prefixLength));
}

// explicit IP(const QString &ipString, int prefixLength = 128, QObject *parent = nullptr);
// explicit IP(const QByteArray &ipValue, int prefixLength = 128, QObject *parent = nullptr);

IPv4Ptr_t IP<UT::IPVersion::IPv6>::toIPv4() const {
    if (m_ipValue.startsWith(QByteArray::fromHex("00000000000000000000FFFF"))) {
        uint32_t ipv4Value = (static_cast<uint32_t>(static_cast<uint8_t>(m_ipValue[12])) << 24) |
                             (static_cast<uint32_t>(static_cast<uint8_t>(m_ipValue[13])) << 16) |
                             (static_cast<uint32_t>(static_cast<uint8_t>(m_ipValue[14])) << 8) |
                             static_cast<uint32_t>(static_cast<uint8_t>(m_ipValue[15]));
        return IPv4Ptr_t::create(ipv4Value);
    } else {
        throw std::invalid_argument("Not a mapped IPv4 address");
    }
}

IpPtr_t IP<UT::IPVersion::IPv6>::createIpPtr(const QString &ipString, int prefixLength, QObject *parent)
{return IpPtr_t(new IPv6_t(ipString, prefixLength, parent));}

IpPtr_t IP<UT::IPVersion::IPv4>::createIpPtr(const QString &ipString, const QString &subnetMask, QObject *parent)
{return IpPtr_t(new IPv4_t(ipString, subnetMask, parent));}
