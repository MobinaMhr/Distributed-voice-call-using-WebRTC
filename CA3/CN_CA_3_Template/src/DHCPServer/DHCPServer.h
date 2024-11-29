#ifndef DHCPSERVER_H
#define DHCPSERVER_H

#include <QObject>

class DHCPServer : public QObject
{
    Q_OBJECT

public:    // constructors
    explicit DHCPServer(QObject *parent = nullptr);
    ~DHCPServer() override;

public:    // methods
Q_SIGNALS:

public Q_SLOTS:

private:    // methods

private:    // members
};

#endif      // DHCPSERVER_H
