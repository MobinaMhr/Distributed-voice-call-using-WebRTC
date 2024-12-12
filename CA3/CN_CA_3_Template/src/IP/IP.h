#ifndef IP_H
#define IP_H

#include "Globals.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QTextStream>


template <UT::IPVersion version>
class IP;

typedef IP<UT::IPVersion::IPv4> IPv4_t;
typedef IP<UT::IPVersion::IPv6> IPv6_t;
typedef QSharedPointer<IPv4_t>  IPv4Ptr_t;
typedef QSharedPointer<IPv6_t>  IPv6Ptr_t;
const QString DEFAULT_SUBNET_MASK = "255.255.255.255";
const uint32_t DEFAULT_IP_VALUE = std::numeric_limits<uint32_t>::max();
const uint32_t DEFAULT_SUBNET_MASK_VALUE = std::numeric_limits<uint32_t>::max();

class AbstractIP : public QObject
{
    Q_OBJECT

public:
    explicit AbstractIP(QObject *parent = nullptr);

Q_SIGNALS:

protected:
};

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

public:    // constructors
    explicit IP(QObject *parent = nullptr);
    explicit IP(const QString &ipString, const QString &subnetMask = "", QObject *parent = nullptr);
    explicit IP(uint32_t ipValue, const QString &subnetMask = "", QObject *parent = nullptr);//TODO: may change to uint32_t
    ~IP() override;

public:    // methods
    QString toString() const;
    uint32_t toValue() const;

    QString getSubnetMask() const;
    void setSubnetMask(const QString &subnetMask);

    IPv4Ptr_t getGateway() const;
    void setGateway(const IP<UT::IPVersion::IPv4> &gateway);

    QPair<QString, QString> getSubnetRange() const;
    bool isInSubnet(const QString &otherIP) const;

    IPv6_t toIPv6() const;


public:    // operators
    bool
    operator==(const IP<UT::IPVersion::IPv4> &other) const
    {
        return toValue() == other.toValue();
    }

private:    // methods


private:
    uint32_t m_ipValue;
    uint32_t m_subnetMask;
    QSharedPointer<IP<UT::IPVersion::IPv4>> m_gateway;
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

public:    // constructors
    explicit IP(QObject *parent = nullptr);
    explicit IP(const QString &ipString, QObject *parent = nullptr);
    explicit IP(uint64_t ipValue, QObject *parent = nullptr);
    ~IP() override;

public:    // methods


public:    // operators
    bool
    operator==(const IP<UT::IPVersion::IPv6> &other) const
    {
        // return toValue() == other.toValue();
    }

private:    // methods

private:
};

#endif    // IP_H
