#ifndef IPHEADER_H
#define IPHEADER_H

#include <QObject>
#include "./Globals.h"
#include "IP.h"

template <UT::IPVersion version>
class IPHeader;

typedef IPHeader<UT::IPVersion::IPv4> IPHv4_t;
typedef IPHeader<UT::IPVersion::IPv6> IPHv6_t;

class AbstractIPHeader : public QObject
{
    Q_OBJECT

public:
    explicit AbstractIPHeader(QObject *parent = nullptr);
    UT::IPVersion ipVersion() const;
    virtual void print() = 0;

protected: // Getters and Setters
    UT::IPVersion m_ipVersion;

Q_SIGNALS:
};

template <>
class IPHeader<UT::IPVersion::IPv4> : public AbstractIPHeader
{
public:    // constructors
    explicit IPHeader(QObject *parent = nullptr);
    IPHeader(const IPHeader &other)
        : AbstractIPHeader(other.parent())
        , m_versionHeaderLength(other.m_versionHeaderLength)
        , m_typeOfService(other.m_typeOfService)
        , m_totalLength(other.m_totalLength)
        , m_identification(other.m_identification)
        , m_flagsFragmentOffset(other.m_flagsFragmentOffset)
        , m_ttl(other.m_ttl)
        , m_protocol(other.m_protocol)
        , m_headerChecksum(other.m_headerChecksum)
        , m_sourceIp(other.m_sourceIp)
        , m_destIp(other.m_destIp)
    {}
    IPHeader& operator=(const IPHeader& other);

public:    // Getters and Setters
    uint8_t versionHeaderLength() const;
    void setVersionHeaderLength(uint8_t newVersionHeaderLength);

    uint8_t typeOfService() const;
    void setTypeOfService(uint8_t newTypeOfService);

    uint16_t totalLength() const;
    void setTotalLength(uint16_t newTotalLength);

    uint16_t identification() const;
    void setIdentification(uint16_t newIdentification);

    uint16_t flagsFragmentOffset() const;
    void setFlagsFragmentOffset(uint16_t newFlagsFragmentOffset);

    uint8_t ttl() const;
    void setTtl(uint8_t newTtl);

    uint8_t protocol() const;
    void setProtocol(uint8_t newProtocol);

    uint16_t headerChecksum() const;
    void setHeaderChecksum(uint16_t newHeaderChecksum);

    QString sourceIp() const;
    void setSourceIp(IPv4Ptr_t newSourceIp);

    QString destIp() const;
    void setDestIp(IPv4Ptr_t newDestIp);

    void print() override;

private:
    uint8_t     m_versionHeaderLength;
    uint8_t     m_typeOfService;
    uint16_t    m_totalLength;
    uint16_t    m_identification;
    uint16_t    m_flagsFragmentOffset;
    uint8_t     m_ttl;
    uint8_t     m_protocol;
    uint16_t    m_headerChecksum;
    IPv4Ptr_t   m_sourceIp;
    IPv4Ptr_t   m_destIp;


};

//###########################

template <>
class IPHeader<UT::IPVersion::IPv6> : public AbstractIPHeader
{
public:    // constructors
    explicit IPHeader(QObject *parent = nullptr);
    IPHeader(const IPHeader &other)
        : AbstractIPHeader(other.parent())
        , m_versionTrafficClassFlowLabel(other.m_versionTrafficClassFlowLabel)
        , m_payloadLength(other.m_payloadLength)
        , m_nextHeader(other.m_nextHeader)
        , m_hopLimit(other.m_hopLimit)
        , m_sourceIp(other.m_sourceIp)
        , m_destIp(other.m_destIp)
    {}

    IPHeader& operator=(const IPHeader& other);

public:    // Getters and Setters
    uint32_t versionTrafficClassFlowLabel() const;
    void setVersionTrafficClassFlowLabel(uint32_t newVersionTrafficClassFlowLabel);

    uint16_t payloadLength() const;
    void setPayloadLength(uint16_t newPayloadLength);

    uint8_t nextHeader() const;
    void setNextHeader(uint8_t newNextHeader);

    uint8_t hopLimit() const;
    void setHopLimit(uint8_t newHopLimit);

    QString sourceIp() const;
    void setSourceIp(IPv6Ptr_t newSourceIp);

    QString destIp() const;
    void setDestIp(IPv6Ptr_t newDestIp);

    void print() override;

private:  // members
    uint32_t    m_versionTrafficClassFlowLabel;
    uint16_t    m_payloadLength;
    uint8_t     m_nextHeader;
    uint8_t     m_hopLimit;
    IPv6Ptr_t   m_sourceIp;
    IPv6Ptr_t   m_destIp;
};

#endif    // IPHEADER_H
