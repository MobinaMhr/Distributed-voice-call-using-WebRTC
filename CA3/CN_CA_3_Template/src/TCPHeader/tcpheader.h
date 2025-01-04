#ifndef TCPHEADER_H
#define TCPHEADER_H

#include <QObject>
#include <QDebug>

class TCPHeader : public QObject {
    Q_OBJECT
public:
    explicit TCPHeader(uint16_t srcPort, uint16_t destPort, QObject *parent = nullptr);
    TCPHeader(const TCPHeader &other);
    TCPHeader& operator=(const TCPHeader &other);
    TCPHeader(TCPHeader &&other) noexcept;
    TCPHeader& operator=(TCPHeader &&other) noexcept;

    uint16_t sourcePort() const;
    uint16_t destinationPort() const;
    uint32_t sequenceNumber() const;
    uint32_t acknowledgmentNumber() const;
    uint8_t dataOffset() const;
    uint8_t flags() const;
    uint16_t windowSize() const;
    uint16_t checksum() const;
    uint16_t urgentPointer() const;

    void setSourcePort(uint16_t port);
    void setDestPort(uint16_t port);
    void setSequenceNumber(uint32_t sequenceNumber);
    void setAcknowledgmentNumber(uint32_t acknowledgmentNumber);
    void setDataOffset(uint8_t dataOffset);
    void setFlags(uint8_t flags);
    void setWindowSize(uint16_t windowSize);
    void setChecksum(uint16_t checksum);
    void setUrgentPointer(uint16_t urgentPointer);

    void print();
    QString getLogMessage();

private:
    uint16_t    m_sourcePort;
    uint16_t    m_destPort;
    uint32_t    m_sequenceNumber;
    uint32_t    m_acknowledgmentNumber;
    uint8_t     m_dataOffset;
    uint8_t     m_flags;
    uint16_t    m_windowSize;
    uint16_t    m_checksum;
    uint16_t    m_urgentPointer;

};

#endif // TCPHEADER_H
