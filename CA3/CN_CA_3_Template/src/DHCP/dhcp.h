#ifndef DHCP_H
#define DHCP_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QSharedPointer>
#include <QMap>
#include "../Node/node.h"
// #include "../Router/router.h"
#include "../IP/IP.h"

class DHCP : public QObject
{
    Q_OBJECT

public:
    explicit DHCP(int asNumber, QObject *parent = nullptr);
    ~DHCP() override;
    QString assignIPToNode(const int &id);

private:
    int m_asNumber;
    QFile m_outputFile;
    QTextStream m_outputStream;
    QMap<QString, IpPtr_t> m_assignedIPs; // Key: MACAddress, Value: IP

    IpPtr_t generateIPv4();
    IpPtr_t generateIPv6();
    void writeToFile(const QString& id, const QString& ip, const QString& macAddress, UT::IPVersion version);
    IpPtr_t getSuitableIPAddress(const QSharedPointer<Node>& node);
    IpPtr_t createIPAddress(UT::IPVersion version, const QString &ipString, const QString &subnetMaskOrPrefix = "");
};

#endif // DHCP_H
