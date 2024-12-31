# DHCP

This is the constructor for the DHCP class. It initializes the Autonomous System (AS) number and sets up the output file for logging DHCP activities. If the file cannot be opened for writing, a warning is issued. The output stream is then set to use this file.

```cpp
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
```

This is the destructor for the DHCP class. It ensures that the output file is properly closed when the DHCP object is destroyed.

```cpp
DHCP::~DHCP()
{
    m_outputFile.close();
}
```

This method retrieves or creates a suitable IP address for a given node. It checks if the node's MAC address already has an assigned IP address. If so, it returns the existing IP address. Otherwise, it generates a new IP address (either IPv4 or IPv6) based on the node's IP version and assigns it to the MAC address.

```cpp
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
```

This method assigns an IP address to a node based on its ID. It uses a template to format the IP address, which varies depending on whether the ID is less than 10. The assigned IP address is then logged and returned.

```cpp
QString DHCP::assignIPToNode(const int &id)
{
    QString ipTemplate = (id < 10) ? "198.168.%100.%1" : "198.168.%100.%2";
    QString formattedIp = ipTemplate.arg(m_asNumber).arg(id);
    qDebug() << "Assigned IP" << formattedIp << "to Node ID:" << id ;
    return formattedIp;
}
```

This method generates a new IPv4 address. It uses a static variable to keep track of the last octet used. If the pool of available IPv4 addresses is exhausted, a warning is issued, and nullptr is returned. Otherwise, a new IPv4 address is created and returned.

```cpp
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
```

This method generates a new IPv6 address. It uses a unique local address prefix and embeds the AS number in the address. A static variable is used to keep track of the last interface ID used. A new IPv6 address is created and returned.

```cpp
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
```

This method creates an IP address based on the specified IP version (IPv4 or IPv6), IP string, and subnet mask or prefix length. It returns a pointer to the created IP address. If the IP version is unsupported, a warning is issued, and nullptr is returned.

```cpp
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
```

This method writes DHCP log information to the output file. It logs the AS number, node ID, IP address, MAC address, and IP version. If the output file is not open for writing, a warning is issued.
```cpp
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
```
