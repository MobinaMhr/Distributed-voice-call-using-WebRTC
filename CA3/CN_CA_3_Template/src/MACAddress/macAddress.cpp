#include "macAddress.h"

MacAddress::MacAddress(uint64_t address) : m_address(address){
    if (!validate())//TODO: add loging;
        throw std::invalid_argument("Invalid MAC address");
}

MacAddress::MacAddress(const QString &address): m_address(parseAddress(address)){
    if (!validate())//TODO: add loging;
        throw std::invalid_argument("Invalid MAC address");
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
