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

Q_SIGNALS:
};

template <>
class IPHeader<UT::IPVersion::IPv4> : public AbstractIPHeader
{
public:    // constructors
    explicit IPHeader(QObject *parent = nullptr);

private:
    uint8_t  m_versionHeaderLength;
    uint8_t  m_typeOfService;
    uint16_t m_totalLength;
    uint16_t m_identification;
    uint16_t m_flagsFragmentOffset;
    uint8_t  m_ttl;
    uint8_t  m_protocol;
    uint16_t m_headerChecksum;
    IPv4Ptr_t m_sourceIp;
    IPv4Ptr_t m_destIp;


};

//###########################

template <>
class IPHeader<UT::IPVersion::IPv6> : public AbstractIPHeader
{
public:    // constructors
    explicit IPHeader(QObject *parent = nullptr);

private:  // members
    uint32_t m_versionTrafficClassFlowLabel;
    uint16_t m_payloadLength;
    uint8_t  m_nextHeader;
    uint8_t  m_hopLimit;
    IPv6Ptr_t m_sourceIP;
    IPv6Ptr_t m_destIP;
};

#endif    // IPHEADER_H
