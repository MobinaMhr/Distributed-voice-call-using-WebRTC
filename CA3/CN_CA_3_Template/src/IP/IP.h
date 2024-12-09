// #ifndef IP_H
// #define IP_H

// #include "Globals.h"

// #include <QObject>
// #include <QSharedPointer>
// #include <QString>
// #include <QTextStream>

// template <UT::IPVersion version>
// class IP;

// typedef IP<UT::IPVersion::IPv4> IPv4_t;
// typedef IP<UT::IPVersion::IPv6> IPv6_t;
// typedef QSharedPointer<IPv4_t>  IPv4Ptr_t;
// typedef QSharedPointer<IPv6_t>  IPv6Ptr_t;

// class AbstractIP : public QObject
// {
//     Q_OBJECT

// public:
//     explicit AbstractIP(QObject *parent = nullptr);

// Q_SIGNALS:

// protected:
// };

// /**
//  * ===========================================
//  * ===========================================
//  * ===========================================
//  * @brief The IP class for IPv4.
//  * ===========================================
//  * ===========================================
//  * ===========================================
//  */
// template <>
// class IP<UT::IPVersion::IPv4> : public AbstractIP
// {
//     // removed Q_OBJECT macro to avoid moc error
//     // https://doc.qt.io/qt-6/moc.html
//     // Q_OBJECT

// public:    // constructors
//     explicit IP(QObject *parent = nullptr);
//     explicit IP(const QString &ipString, QObject *parent = nullptr);
//     explicit IP(uint64_t ipValue, QObject *parent = nullptr);
//     ~IP() override;

// public:    // methods


// public:    // operators
//     bool
//     operator==(const IP<UT::IPVersion::IPv4> &other) const
//     {
//         return toValue() == other.toValue();
//     }

// private:    // methods


// private:
// };

// /**
//  * ===========================================
//  * ===========================================
//  * ===========================================
//  * @brief The IP class for IPv6.
//  * ===========================================
//  * ===========================================
//  * ===========================================
//  */
// template <>
// class IP<UT::IPVersion::IPv6> : public AbstractIP
// {
//     // removed Q_OBJECT macro to avoid moc error
//     // https://doc.qt.io/qt-6/moc.html
//     // Q_OBJECT

// public:    // constructors
//     explicit IP(QObject *parent = nullptr);
//     explicit IP(const QString &ipString, QObject *parent = nullptr);
//     explicit IP(uint64_t ipValue, QObject *parent = nullptr);
//     ~IP() override;

// public:    // methods


// public:    // operators
//     bool
//     operator==(const IP<UT::IPVersion::IPv6> &other) const
//     {
//         return toValue() == other.toValue();
//     }

// private:    // methods

// private:
// };

// #endif    // IP_H
