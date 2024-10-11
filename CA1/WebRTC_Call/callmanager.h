#ifndef CALLMANAGER_H
#define CALLMANAGER_H

#include <QObject>
#include <QString>

class CallManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString callerId READ callerId WRITE setCallerId NOTIFY callerIdChanged)

public:
    explicit CallManager(QObject *parent = nullptr);

    QString callerId() const;

    void setCallerId(const QString &callerId);

    Q_INVOKABLE void startCall();

    Q_INVOKABLE void endCall();

signals:
    void callerIdChanged();

private:
    QString m_callerId;
};

#endif // CALLMANAGER_H
