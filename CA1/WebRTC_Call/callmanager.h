#ifndef CALLMANAGER_H
#define CALLMANAGER_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <webrtc.h>
#include <socket.h>
#include <audioinput.h>
#include <audiooutput.h>

class CallManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString ipAddress READ ipAddress WRITE setIpAddress NOTIFY ipAddressChanged)
    Q_PROPERTY(QString iceCandidate READ iceCandidate WRITE setIceCandidate NOTIFY iceCandidateChanged)
    Q_PROPERTY(QString callerId READ callerId WRITE setCallerId NOTIFY callerIdChanged)
    Q_PROPERTY(QString m_userName READ userName WRITE setUserName NOTIFY userNameChanged)

public:
    explicit CallManager(QObject *parent = nullptr);

    ~CallManager();

    QString ipAddress() const;
    QString iceCandidate() const;
    QString callerId() const;
    QString userName() const;

    void setIpAddress(const QString &ipAddress);
    void setIceCandidate(const QString &iceCandidate);

    Q_INVOKABLE void setCallerId(const QString &callerId);
    Q_INVOKABLE void setUserName(const QString &userName);
    Q_INVOKABLE void startCall();
    Q_INVOKABLE void endCall();
    Q_INVOKABLE void registerUser();

signals:
    void ipAddressChanged();
    void iceCandidateChanged();
    void callerIdChanged();
    void userNameChanged();

private:
    QString m_ipAddress;
    QString m_iceCandidate;
    QString m_callerId;
    QString m_userName;
    QString webrtcPeerId;
    QString m_candidate;
    QString candidate_mid;

    WebRTC* webrtc;

    Socket* socket;

    AudioInput* audioInput;
    AudioOutput* audioOutput;

    QString getCompletedJson(const QString& description, const QString type);
    QString createJsonRequest(const std::vector<QString> &keys, const std::vector<QString> &values);

    QJsonArray getCandidatesQJsonArr(std::vector<rtc::Candidate> candidates);

    // void createWebRTC(const QString &id, bool isOfferer);
    void handleSingalingOffer(const QJsonObject &offer);
    void handleSingalingAnswer(const QJsonObject &answer);

    bool callStartedComeOn;

    std::vector<rtc::Candidate> extractCandidates (const QString &sdp);

private slots:
    void handleIncomingSocketMessage(const QString &message);
};

#endif // CALLMANAGER_H
