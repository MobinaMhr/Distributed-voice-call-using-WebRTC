#include "socket.h"

Socket::Socket(const QUrl &url, QObject *parent)
: QObject(parent), m_url(url) {
    m_webSocket = new QWebSocket();
}

void Socket::connectToServer()
{
    m_webSocket->open(m_url);
}

void Socket::disconnectFromServer()
{
    m_webSocket->close();
}
