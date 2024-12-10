#ifndef MACADDRESS_H
#define MACADDRESS_H

#include <QObject>
#include <QString>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <QRegularExpression>


class MacAddress {
public:
    explicit MacAddress(uint64_t address);
    explicit MacAddress(const QString &address);
    QString toString() const;

private:
    uint64_t m_address;
    static const uint64_t MAX_MAC_ADDRESS = 0xFFFFFFFFFFFF;

    bool validate() const;
    uint64_t parseAddress(const QString &address) const;
};

// class MACAddressGenerator : public QObject {
//     Q_OBJECT
// public:
//     explicit MACAddressGenerator(QObject *parent = nullptr);
//     MACAddress generate();

// private:
//     uint64_t m_counter;
// };

#endif // MACADDRESS_H
