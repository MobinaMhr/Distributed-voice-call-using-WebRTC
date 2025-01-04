#include "ipheader.h"

AbstractIPHeader::AbstractIPHeader(QObject *parent) :
    QObject {parent}
{}

UT::IPVersion AbstractIPHeader::ipVersion() const{
    return m_ipVersion;
}

IPHeader<UT::IPVersion::IPv4>::IPHeader(QObject *parent)
    : AbstractIPHeader(parent),
    m_versionHeaderLength(IPV4_HEADER_LENGTH),
    m_typeOfService(0),
    m_totalLength(0),
    m_identification(0),
    m_flagsFragmentOffset(0),
    m_ttl(0),
    m_protocol(0),
    m_headerChecksum(0),
    m_sourceIp(nullptr),
    m_destIp(nullptr)
{}

// Getters and Setters for IPv4 Header
uint8_t IPHeader<UT::IPVersion::IPv4>::versionHeaderLength() const {
    return m_versionHeaderLength;
}

void IPHeader<UT::IPVersion::IPv4>::setVersionHeaderLength(uint8_t newVersionHeaderLength) {
    m_versionHeaderLength = newVersionHeaderLength;
}

uint8_t IPHeader<UT::IPVersion::IPv4>::typeOfService() const {
    return m_typeOfService;
}

void IPHeader<UT::IPVersion::IPv4>::setTypeOfService(uint8_t newTypeOfService) {
    m_typeOfService = newTypeOfService;
}

uint16_t IPHeader<UT::IPVersion::IPv4>::totalLength() const {
    return m_totalLength;
}

void IPHeader<UT::IPVersion::IPv4>::setTotalLength(uint16_t newTotalLength) {
    m_totalLength = newTotalLength;
}

uint16_t IPHeader<UT::IPVersion::IPv4>::identification() const {
    return m_identification;
}

void IPHeader<UT::IPVersion::IPv4>::setIdentification(uint16_t newIdentification) {
    m_identification = newIdentification;
}

uint16_t IPHeader<UT::IPVersion::IPv4>::flagsFragmentOffset() const {
    return m_flagsFragmentOffset;
}

void IPHeader<UT::IPVersion::IPv4>::setFlagsFragmentOffset(uint16_t newFlagsFragmentOffset) {
    m_flagsFragmentOffset = newFlagsFragmentOffset;
}

uint8_t IPHeader<UT::IPVersion::IPv4>::ttl() const {
    return m_ttl;
}

void IPHeader<UT::IPVersion::IPv4>::setTtl(uint8_t newTtl) {
    m_ttl = newTtl;
}

uint8_t IPHeader<UT::IPVersion::IPv4>::protocol() const {
    return m_protocol;
}

void IPHeader<UT::IPVersion::IPv4>::setProtocol(uint8_t newProtocol) {
    m_protocol = newProtocol;
}

uint16_t IPHeader<UT::IPVersion::IPv4>::headerChecksum() const {
    return m_headerChecksum;
}

void IPHeader<UT::IPVersion::IPv4>::setHeaderChecksum(uint16_t newHeaderChecksum) {
    m_headerChecksum = newHeaderChecksum;
}

QString IPHeader<UT::IPVersion::IPv4>::sourceIp() const {
    return m_sourceIp->toString();
    // return QString::fromStdString(m_sourceIp->toString());
}

void IPHeader<UT::IPVersion::IPv4>::setSourceIp(IPv4Ptr_t newSourceIp) {
    m_sourceIp = newSourceIp;
}

QString IPHeader<UT::IPVersion::IPv4>::destIp() const {
    return m_destIp->toString();
    // return QString::fromStdString(m_sourceIp->toString());
    // return m_destIp;
}

void IPHeader<UT::IPVersion::IPv4>::setDestIp(IPv4Ptr_t newDestIp) {
    m_destIp = newDestIp;
}

// Constructor for IPv6 Header
IPHeader<UT::IPVersion::IPv6>::IPHeader(QObject *parent)
    : AbstractIPHeader(parent),
    m_versionTrafficClassFlowLabel(0),
    m_payloadLength(0),
    m_nextHeader(0),
    m_hopLimit(0),
    m_sourceIp(nullptr),
    m_destIp(nullptr)
{}

// Getters and Setters for IPv6 Header
uint32_t IPHeader<UT::IPVersion::IPv6>::versionTrafficClassFlowLabel() const {
    return m_versionTrafficClassFlowLabel;
}

void IPHeader<UT::IPVersion::IPv6>::setVersionTrafficClassFlowLabel(uint32_t newVersionTrafficClassFlowLabel) {
    m_versionTrafficClassFlowLabel = newVersionTrafficClassFlowLabel;
}

uint16_t IPHeader<UT::IPVersion::IPv6>::payloadLength() const {
    return m_payloadLength;
}

void IPHeader<UT::IPVersion::IPv6>::setPayloadLength(uint16_t newPayloadLength) {
    m_payloadLength = newPayloadLength;
}

uint8_t IPHeader<UT::IPVersion::IPv6>::nextHeader() const {
    return m_nextHeader;
}

void IPHeader<UT::IPVersion::IPv6>::setNextHeader(uint8_t newNextHeader) {
    m_nextHeader = newNextHeader;
}

uint8_t IPHeader<UT::IPVersion::IPv6>::hopLimit() const {
    return m_hopLimit;
}

void IPHeader<UT::IPVersion::IPv6>::setHopLimit(uint8_t newHopLimit) {
    m_hopLimit = newHopLimit;
}

QString IPHeader<UT::IPVersion::IPv6>::sourceIp() const {
    return m_sourceIp->toString();
}

void IPHeader<UT::IPVersion::IPv6>::setSourceIp(IPv6Ptr_t newSourceIp) {
    m_sourceIp = newSourceIp;
}

QString IPHeader<UT::IPVersion::IPv6>::destIp() const {
    return m_destIp->toString();
}

void IPHeader<UT::IPVersion::IPv6>::setDestIp(IPv6Ptr_t newDestIp) {
    m_destIp = newDestIp;
}

void IPHeader<UT::IPVersion::IPv6>::print()
{
    qDebug() << "VersionTrafficClassFlow : " << m_versionTrafficClassFlowLabel;
    qDebug() << "Payload length : " << m_payloadLength;
    qDebug() << "NextHeader : " << m_nextHeader ;
    qDebug() << "HopLimit : " << m_hopLimit ;
    qDebug() << "Source IP : " << m_sourceIp->toString() ;
    qDebug() << "Destenation IP : " << m_destIp->toString() ;

}

void IPHeader<UT::IPVersion::IPv4>::print()
{
    qDebug() << "VersionHeaderLength : " << m_versionHeaderLength;
    qDebug() << "TypeOfService : " << m_typeOfService;
    qDebug() << "TotalLength : " << m_totalLength;
    qDebug() << "Identification : " << m_identification;
    qDebug() << "FlagsFragmentOffset : " << m_flagsFragmentOffset;
    qDebug() << "Ttl : " << m_ttl;
    qDebug() << "Protocol : " << m_protocol;
    qDebug() << "HeaderChecksum : " << m_headerChecksum;
    qDebug() << "Source IP : " << m_sourceIp->toString() ;
    qDebug() << "Destenation IP : " << m_destIp->toString() ;
}

IPHeader<UT::IPVersion::IPv4> &IPHeader<UT::IPVersion::IPv4>::operator=(const IPHeader<UT::IPVersion::IPv4> &other)
{
    if (this != &other) { // Check for self-assignment
        AbstractIPHeader::setParent(other.parent()); // Copy parent QObject
        m_versionHeaderLength = other.m_versionHeaderLength;
        m_typeOfService = other.m_typeOfService;
        m_totalLength = other.m_totalLength;
        m_identification = other.m_identification;
        m_flagsFragmentOffset = other.m_flagsFragmentOffset;
        m_ttl = other.m_ttl;
        m_protocol = other.m_protocol;
        m_headerChecksum = other.m_headerChecksum;
        m_sourceIp = other.m_sourceIp;
        m_destIp = other.m_destIp;
    }
    return *this;
}

IPHeader<UT::IPVersion::IPv6> &IPHeader<UT::IPVersion::IPv6>::operator=(const IPHeader<UT::IPVersion::IPv6> &other)
{
    if (this != &other) { // Check for self-assignment
        AbstractIPHeader::setParent(other.parent()); // Copy parent QObject
        m_versionTrafficClassFlowLabel = other.m_versionTrafficClassFlowLabel;
        m_payloadLength = other.m_payloadLength;
        m_nextHeader = other.m_nextHeader;
        m_hopLimit = other.m_hopLimit;
        m_sourceIp = other.m_sourceIp;
        m_destIp = other.m_destIp;
    }
    return *this;
}

QString IPHeader<UT::IPVersion::IPv4>::getLogMessage()
{
    QString logMessage = QString(
                           "VersionHeaderLength: %1\n"
                           "TypeOfService: %2\n"
                           "TotalLength: %3\n"
                           "Identification: %4\n"
                           "FlagsFragmentOffset: %5\n"
                           "TTL: %6\n"
                           "Protocol: %7\n"
                           "HeaderChecksum: %8\n"
                           "Source IP: %9\n"
                           "Destination IP: %10")
                           .arg(m_versionHeaderLength)
                           .arg(m_typeOfService)
                           .arg(m_totalLength)
                           .arg(m_identification)
                           .arg(m_flagsFragmentOffset)
                           .arg(m_ttl)
                           .arg(m_protocol)
                           .arg(m_headerChecksum)
                           .arg(m_sourceIp->toString())
                           .arg(m_destIp->toString());

    return logMessage;
}

QString IPHeader<UT::IPVersion::IPv6>::getLogMessage()
{
    QString logMessage = QString("VersionTrafficClassFlow: %1\n"
                           "Payload length: %2\n"
                           "NextHeader: %3\n"
                           "HopLimit: %4\n"
                           "Source IP: %5\n"
                           "Destination IP: %6")
                           .arg(m_versionTrafficClassFlowLabel)
                           .arg(m_payloadLength)
                           .arg(m_nextHeader)
                           .arg(m_hopLimit)
                           .arg(m_sourceIp->toString())
                           .arg(m_destIp->toString());
    return logMessage;
}
