#ifndef PACKET_H
#define PACKET_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include "../Globals/globals.h"
// #include "../IP/IP.h"
#include "../TCPHeader/tcpheader.h"
#include "../DataLinkHeader/datalinkheader.h"

class Packet : public QObject {
    Q_OBJECT

public:
    explicit Packet(UT::PacketType packetType, UT::PacketControlType controlType, quint32 seqNumber,
                    quint32 waitCycles, quint32 totalCycles, const QString destIP, const QByteArray &payload,
                    const DataLinkHeader &dataLinkHeader, const TCPHeader &tcpHeader, QObject *parent);

    void updatePath(const QString& ipAddress);

    void increaseWaitingCyclesBy(int additionalCycles);
    void increaseTotalCyclesBy(int additionalCycles);

    void increaseWaitingCycles();
    void increaseTotalCycles();

    // Getters
    UT::PacketType packetType() const;
    UT::PacketControlType controlType() const;
    QStringList path() const;
    quint32 sequenceNumber() const;
    quint32 waitingCycles() const;
    quint32 totalCycles() const;
    QString destinationIP() const;
    QByteArray payload() const;
    const DataLinkHeader& dataLinkHeader() const;
    const TCPHeader& tcpHeader() const;

    // Setters
    void setPacketType(UT::PacketType packetType);
    void setControlType(UT::PacketControlType controlType);
    void setSequenceNumber(quint32 seqNumber);
    void setDestinationIP(QString destIP);
    void setPayload(const QByteArray &payload);
    void setDataLinkHeader(const DataLinkHeader &dataLinkHeader);
    void setTcpHeader(const TCPHeader &tcpHeader);

private:
    UT::PacketType m_packetType;
    UT::PacketControlType m_controlType;

    QString m_destinationIP;
    // IP and BGP header
    TCPHeader m_tcpHeader;
    DataLinkHeader m_dataLinkHeader;
    QByteArray m_payload;
    quint32 m_sequenceNumber;
    quint32 m_waitingCycles;
    quint32 m_totalCycles;
    QStringList m_path;
};

#endif // PACKET_H
