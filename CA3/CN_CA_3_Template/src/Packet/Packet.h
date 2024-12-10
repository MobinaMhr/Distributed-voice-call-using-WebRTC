#ifndef PACKET_H
#define PACKET_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include "../TCPHeader/tcpheader.h"
#include "../Globals/globals.h"
#include "../DataLinkHeader/datalinkheader.h"

class Packet : public QObject {
    Q_OBJECT

public:
    explicit Packet(UT::PacketType packetType, UT::PacketControlType controlType, quint32 seqNumber, quint32 waitCycles,
                    quint32 totalCycles, const QString destIP, const QByteArray &payload,
                    const DataLinkHeader &dataLinkHeader, const TCPHeader &tcpHeader, QObject *parent);

    void updateRoute(const QString& ipAddress);
    void increaseWaitingCyclesByOne();
    void increaseTotalCyclesByOne();

    // Getters
    UT::PacketType packetType() const;
    UT::PacketControlType controlType() const;
    QStringList route() const;
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
    QStringList m_route;
    UT::PacketType m_packetType;
    UT::PacketControlType m_controlType;

    quint32 m_sequenceNumber;
    quint32 m_waitingCycles;
    quint32 m_totalCycles;

    QString m_destinationIP;
    QByteArray m_payload;
    DataLinkHeader m_dataLinkHeader;
    TCPHeader m_tcpHeader;
};

#endif // PACKET_H
