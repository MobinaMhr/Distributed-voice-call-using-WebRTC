#ifndef CALLMANAGER_H
#define CALLMANAGER_H

#include <QObject>
#include <QString>

class CallManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString ipAddress READ ipAddress WRITE setIpAddress NOTIFY ipAddressChanged)
    Q_PROPERTY(QString iceCandidate READ iceCandidate WRITE setIceCandidate NOTIFY iceCandidateChanged)
    Q_PROPERTY(QString callerId READ callerId WRITE setCallerId NOTIFY callerIdChanged)

public:
    explicit CallManager(QObject *parent = nullptr);

    QString ipAddress() const;

    QString iceCandidate() const;

    QString callerId() const;

    void setIpAddress(const QString &ipAddress);

    void setIceCandidate(const QString &iceCandidate);

    void setCallerId(const QString &callerId);

    Q_INVOKABLE void startCall();

    Q_INVOKABLE void endCall();

signals:
    void ipAddressChanged();

    void iceCandidateChanged();

    void callerIdChanged();

private:
    QString m_ipAddress;

    QString m_iceCandidate;

    QString m_callerId;
};

#endif // CALLMANAGER_H
