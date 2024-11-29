#include "DHCPServer.h"

DHCPServer::DHCPServer(QObject *parent) :
    QObject {parent}
{
    connect(this, &DHCPServer::sendOffer, this, &DHCPServer::sendPacket);
}

DHCPServer::~DHCPServer() {}