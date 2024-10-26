#include "socket.h"

Socket::Socket(const QUrl &url, QObject *parent)
: QObject(parent), m_url(url) {
    m_webSocket = new QWebSocket();
}
