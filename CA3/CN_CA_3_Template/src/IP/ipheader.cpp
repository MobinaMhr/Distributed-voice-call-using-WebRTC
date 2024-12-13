#include "ipheader.h"

AbstractIPHeader::AbstractIPHeader(QObject *parent) :
    QObject {parent}
{}

UT::IPVersion AbstractIPHeader::ipVersion() const{
    return m_ipVersion;
}

IPHeader<UT::IPVersion::IPv4>::IPHeader(QObject *parent)
    : AbstractIPHeader(parent),
    m_versionHeaderLength(0),
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
