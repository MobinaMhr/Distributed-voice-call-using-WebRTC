#include "TCPHeader.h"

TCPHeader::TCPHeader(uint16_t srcPort, uint16_t destPort, QObject *parent)
    : QObject(parent), m_sourcePort(srcPort), m_destPort(destPort) {}

TCPHeader::TCPHeader(const TCPHeader &other) : QObject(other.parent()),
    m_sourcePort(other.m_sourcePort), m_destPort(other.m_destPort),
    m_sequenceNumber(other.m_sequenceNumber), m_acknowledgmentNumber(other.m_acknowledgmentNumber),
    m_dataOffset(other.m_dataOffset), m_flags(other.m_flags), m_windowSize(other.m_windowSize),
    m_checksum(other.m_checksum), m_urgentPointer(other.m_urgentPointer) {}

TCPHeader& TCPHeader::operator=(const TCPHeader &other) {
    if (this != &other) {
        m_sourcePort = other.m_sourcePort;
        m_destPort = other.m_destPort;
        m_sequenceNumber = other.m_sequenceNumber;
        m_acknowledgmentNumber = other.m_acknowledgmentNumber;
        m_dataOffset = other.m_dataOffset;
        m_flags = other.m_flags;
        m_windowSize = other.m_windowSize;
        m_checksum = other.m_checksum;
        m_urgentPointer = other.m_urgentPointer;
        setParent(other.parent());
    }
    return *this;
}

TCPHeader::TCPHeader(TCPHeader &&other) noexcept : QObject(other.parent()),
    m_sourcePort(other.m_sourcePort), m_destPort(other.m_destPort),
    m_sequenceNumber(other.m_sequenceNumber),
    m_acknowledgmentNumber(other.m_acknowledgmentNumber),
    m_dataOffset(other.m_dataOffset), m_flags(other.m_flags), m_windowSize(other.m_windowSize),
    m_checksum(other.m_checksum), m_urgentPointer(other.m_urgentPointer) {
    other.setParent(nullptr);
}

TCPHeader& TCPHeader::operator=(TCPHeader &&other) noexcept {
    if (this != &other) {
        m_sourcePort = other.m_sourcePort;
        m_destPort = other.m_destPort;
        m_sequenceNumber = other.m_sequenceNumber;
        m_acknowledgmentNumber = other.m_acknowledgmentNumber;
        m_dataOffset = other.m_dataOffset;
        m_flags = other.m_flags;
        m_windowSize = other.m_windowSize;
        m_checksum = other.m_checksum;
        m_urgentPointer = other.m_urgentPointer;
        setParent(other.parent());
        other.setParent(nullptr);
    }
    return *this;
}

uint16_t TCPHeader::sourcePort() const {
    return this->m_sourcePort;
}

uint16_t TCPHeader::destinationPort() const {
    return this->m_destPort;
}

uint32_t TCPHeader::sequenceNumber() const
{
    return this->m_sequenceNumber;
}

uint32_t TCPHeader::acknowledgmentNumber() const
{
    return this->m_acknowledgmentNumber;
}

uint8_t TCPHeader::dataOffset() const
{
    return this->m_dataOffset;
}

uint8_t TCPHeader::flags() const
{
    return this->m_flags;
}

uint16_t TCPHeader::windowSize() const
{
    return this->m_windowSize;
}

uint16_t TCPHeader::checksum() const
{
    return this->m_checksum;
}

uint16_t TCPHeader::urgentPointer() const
{
    return this->m_urgentPointer;
}

void TCPHeader::setSourcePort(uint16_t port)
{
    this->m_sourcePort = port;
}

void TCPHeader::setDestPort(uint16_t port)
{
    this->m_destPort = port;
}

void TCPHeader::setSequenceNumber(uint32_t sequenceNumber)
{
    this->m_sequenceNumber = sequenceNumber;
}

void TCPHeader::setAcknowledgmentNumber(uint32_t acknowledgmentNumber)
{
    this->m_acknowledgmentNumber = acknowledgmentNumber;
}

void TCPHeader::setDataOffset(uint8_t dataOffset)
{
    this->m_dataOffset = dataOffset;
}

void TCPHeader::setFlags(uint8_t flags)
{
    this->m_flags = flags;
}

void TCPHeader::setWindowSize(uint16_t windowSize)
{
    this->m_windowSize = windowSize;
}

void TCPHeader::setChecksum(uint16_t checksum)
{
    this->m_checksum = checksum;
}

void TCPHeader::setUrgentPointer(uint16_t urgentPointer)
{
    this->m_urgentPointer = urgentPointer;
}
