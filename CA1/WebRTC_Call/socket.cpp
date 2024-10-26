#include "socket.h"

Socket::Socket(const QUrl &url, QObject *parent)
: QObject(parent), m_url(url) {
    m_webSocket = new QWebSocket();
    connect(m_webSocket, &QWebSocket::connected, this, &Socket::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &Socket::onDisconnected);
}

void Socket::connectToServer()
{
    m_webSocket->open(m_url);
}

void Socket::disconnectFromServer()
{
    m_webSocket->close();
}

void Socket::onConnected()
{
    emit connected();
}

void Socket::onDisconnected()
{
    emit disconnected();
}

void Socket::onMessageReceived(const QString &message)
{
    emit messageReceived(message);
}
