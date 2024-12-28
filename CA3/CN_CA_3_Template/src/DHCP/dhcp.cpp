#include "dhcp.h"
#include <QDebug>

DHCP::DHCP(int asNumber, QObject *parent)
    : QObject(parent),
    m_asNumber(asNumber),
    m_outputFile(QString("AS_%1_DHCP_Logs.txt").arg(asNumber))
{
    if (!m_outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open DHCP log file for writing.";
    }
    m_outputStream.setDevice(&m_outputFile);
}

DHCP::~DHCP()
{
    m_outputFile.close();
}

IpPtr_t DHCP::getSuitableIPAddress(const QSharedPointer<Node>& node)
{
    // Retrieve or create an IP address
    IpPtr_t ipAddress;
    QString macAddress = node->macAddress().toString();

    if (m_assignedIPs.contains(macAddress)) {
        ipAddress = m_assignedIPs.value(macAddress);
    } else {
        ipAddress = (node->ipVersion() == UT::IPVersion::IPv4) ? generateIPv4() : generateIPv6();
        m_assignedIPs.insert(macAddress, ipAddress);
    }

    return ipAddress;
}

QString DHCP::assignIPToNode(const int &id)
{
    QString ipTemplate = (id < 10) ? "198.168.%100.%1" : "198.168.%100.%2";
    QString formattedIp = ipTemplate.arg(m_asNumber).arg(id);
    qDebug() << "Assigned IP" << formattedIp << "to Node ID:" << id ;
    return formattedIp;
}

IpPtr_t DHCP::generateIPv4()
{
    static uint32_t lastOctet = 1;
    if (lastOctet > 254) {
        qWarning() << "IPv4 address pool exhausted!";
        return nullptr;
    }
    QString ipString = QString("192.168.%1.%2").arg(m_asNumber).arg(lastOctet++);
    return QSharedPointer<IPv4_t>::create(ipString, DEFAULT_SUBNET_MASK);
}

IpPtr_t DHCP::generateIPv6()
{
    QByteArray ipValue(16, 0);
    ipValue[0] = 0xFD; // Unique local address prefix
    ipValue[1] = static_cast<char>(m_asNumber); // Embed AS number
    static uint16_t lastInterfaceId = 1;
    ipValue[15] = lastInterfaceId & 0xFF; // Interface ID
    ipValue[14] = (lastInterfaceId++ >> 8) & 0xFF;
    return QSharedPointer<IPv6_t>::create(ipValue, DEFAULT_IPV6_PREFIX_LENGTH);
}

IpPtr_t DHCP::createIPAddress(UT::IPVersion version, const QString &ipString, const QString &subnetMaskOrPrefix)
{
    switch (version) {
        case UT::IPVersion::IPv4: {
                QString subnetMask = subnetMaskOrPrefix.isEmpty() ? DEFAULT_SUBNET_MASK : subnetMaskOrPrefix;
                return QSharedPointer<IPv4_t>::create(ipString, subnetMask);
            }
        case UT::IPVersion::IPv6: {
                int prefixLength = subnetMaskOrPrefix.isEmpty() ? DEFAULT_IPV6_PREFIX_LENGTH : subnetMaskOrPrefix.toInt();
                return QSharedPointer<IPv6_t>::create(ipString, prefixLength);
            }
        default:
            qWarning() << "Unsupported IP version!";
            return nullptr;
    }
}

void DHCP::writeToFile(const QString& id, const QString& ip, const QString& macAddress, UT::IPVersion version)
{
    if (!m_outputFile.isOpen()) {
        qWarning() << "DHCP log file is not open for writing.";
        return;
    }

    m_outputStream << "AS Number: " << m_asNumber
                   << ", Node ID: " << id
                   << ", IP: " << ip
                   << ", MAC: " << macAddress
                   << ", IP Version: " << (version == UT::IPVersion::IPv4 ? "IPv4" : "IPv6")
                   << "\n";
    m_outputStream.flush();
}
