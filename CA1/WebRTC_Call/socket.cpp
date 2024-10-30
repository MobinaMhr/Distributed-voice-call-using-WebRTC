#include "socket.h"

Socket::Socket(const QUrl &url, QObject *parent)
: QObject(parent), m_url(url) {
    m_webSocket = new QWebSocket();
    connect(m_webSocket, &QWebSocket::connected, this, &Socket::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &Socket::onDisconnected);
    connect(m_webSocket, &QWebSocket::textMessageReceived, this, &Socket::onMessageReceived);
}

Socket::~Socket()
{
    if (m_webSocket)
        delete m_webSocket;
}

void Socket::connectToServer()
{
    m_webSocket->open(m_url);
    qDebug() << "SOCKET(___)" << "I'm connected to that, be happy:> " << m_url;
}

void Socket::disconnectFromServer()
{
    m_webSocket->close();
}

void Socket::sendMessage(const QString &message)
{
    if (m_webSocket->isValid())
        m_webSocket->sendTextMessage(message);

}

bool Socket::isConnected() const
{
    return m_webSocket->state() == QAbstractSocket::ConnectedState;
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
    qDebug() << "SOCKET(___)" << "~:" << message;
    emit messageReceived(message);
}
