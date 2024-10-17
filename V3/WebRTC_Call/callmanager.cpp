#include "callmanager.h"
#include <QDebug>

CallManager::CallManager(QObject *parent)
    : QObject{parent},
    m_ipAddress("172.16.142.176"),
    m_iceCandidate("172.16.142.176")
{
}

QString CallManager::ipAddress() const
{
    return m_ipAddress;
}

QString CallManager::iceCandidate() const
{
    return m_iceCandidate;
}

QString CallManager::callerId() const
{
    return m_callerId;
}

void CallManager::setIpAddress(const QString &ipAddress)
{
    if (m_ipAddress != ipAddress) {
        m_ipAddress = ipAddress;
        emit ipAddressChanged();
    }
}

void CallManager::setIceCandidate(const QString &iceCandidate)
{
    if (m_iceCandidate != iceCandidate) {
        m_iceCandidate = iceCandidate;
        emit iceCandidateChanged();
    }
}

void CallManager::setCallerId(const QString &callerId)
{
    if (m_callerId != callerId) {
        m_callerId = callerId;
        emit callerIdChanged();
    }
}

void CallManager::startCall()
{
    qDebug() << "Starting call with Caller ID:" << m_callerId;
}

void CallManager::endCall()
{
    qDebug() << "Ending call with Caller ID:" << m_callerId;
}


