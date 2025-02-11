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

    void connectToServer();
    void disconnectFromServer();
    void sendMessage(const QString &message);
    bool isConnected() const;

signals:
    void connected();
    void disconnected();
    void messageReceived(const QString &message);

private slots:
    void onConnected();
    void onDisconnected();
    void onMessageReceived(const QString &message);

private:
    QWebSocket *m_webSocket;

    QUrl m_url;
};

#endif // SOCKET_H
