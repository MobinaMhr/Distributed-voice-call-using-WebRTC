#ifndef IP_H
#define IP_H

#include "Globals.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QTextStream>
#include <QHostAddress>
#include <QPair>
#include <QByteArray>

template <UT::IPVersion version>
class IP;

typedef IP<UT::IPVersion::IPv4> IPv4_t;
typedef IP<UT::IPVersion::IPv6> IPv6_t;
typedef QSharedPointer<IPv4_t>  IPv4Ptr_t;
typedef QSharedPointer<IPv6_t>  IPv6Ptr_t;
const QString DEFAULT_SUBNET_MASK = "255.255.255.255";
const uint32_t DEFAULT_IP_VALUE = std::numeric_limits<uint32_t>::max();
const uint32_t DEFAULT_SUBNET_MASK_VALUE = std::numeric_limits<uint32_t>::max();
const int DEFAULT_IPV6_PREFIX_LENGTH = 64;
const int IPV6_Length_IN_BYTES = 16;
const int IPV6_Length_IN_BITS = 128;
const char IPV6_DEFAULT_FILL_VALUE = 0xFF;
const char IPV6_DELIM = ':';
const std::string IPV6_VALUE_ERROR = "Invalid IPv6 format";
const std::string SUBNET_MASK_FORMAT_ERROR = "Invalid subnet mask format";
const std::string EMPTY_GATEWAY_ERROR = "Gateway is not set";
const std::string INVALID_PREFIX_LENGTH_ERROR = "invalid prefix length";

class AbstractIP : public QObject
{
    Q_OBJECT

public:
    explicit AbstractIP(QObject *parent = nullptr);
    // bool operator<(const AbstractIP& other) const;

    virtual IPv4Ptr_t toIPv4() const = 0;
    virtual IPv6Ptr_t toIPv6() const = 0;

    virtual QString toString() const = 0;
Q_SIGNALS:

protected:
};

typedef QSharedPointer<AbstractIP> IpPtr_t;

/**
 * ===========================================
 * ===========================================
 * ===========================================
 * @brief The IP class for IPv4.
 * ===========================================
 * ===========================================
 * ===========================================
 */
template <>
class IP<UT::IPVersion::IPv4> : public AbstractIP
{
    // removed Q_OBJECT macro to avoid moc error
    // https://doc.qt.io/qt-6/moc.html
    // Q_OBJECT

public:
    // IP(const IP<UT::IPVersion::IPv4> &other);
    explicit IP(QObject *parent = nullptr);
    explicit IP(const QString &ipString, const QString &subnetMask = "", QObject *parent = nullptr);
    explicit IP(uint32_t ipValue, const QString &subnetMask = "", QObject *parent = nullptr);//TODO: may change to uint32_t
    ~IP() override;
    IP(const IP&);

public:
    QString toString() const override;
    uint32_t toValue() const;

    QString getSubnetMask() const;
    void setSubnetMask(const QString &subnetMask);

    IPv4Ptr_t getGateway() const;
    void setGateway(IPv4Ptr_t &gateway);

    QPair<QString, QString> getSubnetRange() const;
    bool isInSubnet(const QString &otherIP) const;

    IPv4Ptr_t toIPv4() const override;
    IPv6Ptr_t toIPv6() const override;

    static IpPtr_t createIpPtr(const QString &ipString = "", const QString &subnetMask = "", QObject *parent = nullptr);

public:
    bool operator==(const IP<UT::IPVersion::IPv4> &other) const {
        return toValue() == other.toValue();
    }
    IP<UT::IPVersion::IPv4> &operator=(const IP &other) {
        if (this != &other) {
            m_ipValue = other.m_ipValue;
            m_subnetMask = other.m_subnetMask;
            m_gateway = other.m_gateway;
        }
        return *this;
    }
private:


private:
    uint32_t    m_ipValue;
    uint32_t    m_subnetMask;
    IPv4Ptr_t   m_gateway;
};

/**
 * ===========================================
 * ===========================================
 * ===========================================
 * @brief The IP class for IPv6.
 * ===========================================
 * ===========================================
 * ===========================================
 */
template <>
class IP<UT::IPVersion::IPv6> : public AbstractIP
{
    // removed Q_OBJECT macro to avoid moc error
    // https://doc.qt.io/qt-6/moc.html
    // Q_OBJECT

public:
    // IP(const IP<UT::IPVersion::IPv6> &other);
    explicit IP(QObject *parent = nullptr);
    explicit IP(const QString &ipString, int prefixLength = 128, QObject *parent = nullptr);
    explicit IP(const QByteArray &ipValue, int prefixLength = 128, QObject *parent = nullptr);
    ~IP() override;
    IP(const IP&);

public:    // methods
    QString toString() const override;
    QByteArray toValue() const;

    int getPrefixLength() const;
    void setPrefixLength(int prefixLength);

    IPv6Ptr_t getGateway() const;
    void setGateway(const IPv6Ptr_t &gateway);

    QPair<QString, QString> getSubnetRange() const;
    bool isInSubnet(const QString &otherIP) const;

    IPv4Ptr_t toIPv4() const override;
    IPv6Ptr_t toIPv6() const override;
    static IpPtr_t createIpPtr(const QString &ipString = "", int prefixLength = 128, QObject *parent = nullptr);

public:
    bool operator==(const IP<UT::IPVersion::IPv6> &other) const {
        return toValue() == other.toValue();
    }
    IP<UT::IPVersion::IPv6> &operator=(const IP &other) {
        if (this != &other) {
            m_ipValue = other.m_ipValue;
            m_prefixLength = other.m_prefixLength;
            m_gateway = other.m_gateway;
        }
        return *this;
    }

private:
    QByteArray  m_ipValue;
    int         m_prefixLength;
    IPv6Ptr_t   m_gateway;
};

#endif    // IP_H



