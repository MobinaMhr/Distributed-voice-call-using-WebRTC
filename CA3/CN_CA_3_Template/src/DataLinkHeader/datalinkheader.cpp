#include "DataLinkHeader.h"

DataLinkHeader::DataLinkHeader(const MacAddress &src, const MacAddress &dest, QObject *parent)
    : QObject(parent), m_sourceMACAddress(src), m_destinationMACAddress(dest) {}

MacAddress DataLinkHeader::source() const {
    return m_sourceMACAddress;
}

MacAddress DataLinkHeader::destination() const {
    return m_destinationMACAddress;
}

void DataLinkHeader::setSourceMACAddress(const MacAddress &newSourceMACAddress)
{
    m_sourceMACAddress = newSourceMACAddress;
}

void DataLinkHeader::setDestinationMACAddress(const MacAddress &newDestinationMACAddress)
{
    m_destinationMACAddress = newDestinationMACAddress;
}
