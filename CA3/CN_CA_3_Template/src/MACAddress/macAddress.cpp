#include "macAddress.h"

MacAddress::MacAddress(uint64_t address) : m_address(address){
    if (!validate())//TODO: add loging;
        throw std::invalid_argument("Invalid MAC address");
}

MacAddress::MacAddress(const QString &address): m_address(parseAddress(address)){
    if (!validate())//TODO: add loging;
        throw std::invalid_argument("Invalid MAC address");
}

QString MacAddress::toString() const
{
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(12) << m_address;
    std::string hexString = ss.str();
    QString macString;
    for (size_t i = 0; i < hexString.size(); i += 2) {
        macString.append(QString::fromStdString(hexString.substr(i, 2)));
        if (i < hexString.size() - 2) {
            macString.append(":");
        }
    }
    return macString;
}

bool MacAddress::validate() const
{
    return m_address <= MAX_MAC_ADDRESS;
}

uint64_t MacAddress::parseAddress(const QString &address) const
{
    QRegularExpression re("([0-9A-Fa-f]{2}[:\\-]?){6}");
    QRegularExpressionMatch match = re.match(address);

    if (!match.hasMatch())
        throw std::invalid_argument("Invalid MAC address format");

    QString sanitized = address;
    sanitized.remove(':').remove('-');
    bool ok; uint64_t mac = sanitized.toULongLong(&ok, 16);
    if (!ok)
        throw std::invalid_argument("Failed to parse MAC address");
    return mac;
}

// MACAddressGenerator::MACAddressGenerator(QObject *parent)
//     : QObject(parent), m_counter(0) {}

// MACAddress MACAddressGenerator::generate() {
//     QString mac = QString("00:1A:C2:%1:%2:%3")
//     .arg((m_counter >> 16) & 0xFF, 2, 16, QChar('0'))
//       .arg((m_counter >> 8) & 0xFF, 2, 16, QChar('0'))
//       .arg(m_counter & 0xFF, 2, 16, QChar('0'));
//     ++m_counter;
//     return MACAddress(mac);
// }
