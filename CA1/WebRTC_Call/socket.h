#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QWebSocket>

class Socket : public QObject
{
    Q_OBJECT
public:
    explicit Socket(const QUrl &url, QObject *parent = nullptr);
    ~Socket();

signals:

private:
    QWebSocket *m_webSocket;
    QUrl m_url;
};

#endif // SOCKET_H
