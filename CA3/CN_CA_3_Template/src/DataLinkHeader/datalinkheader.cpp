#include "DataLinkHeader.h"

DataLinkHeader::DataLinkHeader(const MacAddress &src, const MacAddress &dest, QObject *parent)
    : QObject(parent), m_sourceMACAddress(src), m_destinationMACAddress(dest) {}

DataLinkHeader::DataLinkHeader(const DataLinkHeader &other) : QObject(other.parent()),
    m_sourceMACAddress(other.m_sourceMACAddress),
    m_destinationMACAddress(other.m_destinationMACAddress) {}

DataLinkHeader& DataLinkHeader::operator=(const DataLinkHeader &other) {
    if (this != &other) {
        m_sourceMACAddress = other.m_sourceMACAddress;
        m_destinationMACAddress = other.m_destinationMACAddress;
        setParent(other.parent());
    }
    return *this;
}

DataLinkHeader::DataLinkHeader(DataLinkHeader &&other) noexcept : QObject(other.parent()),
    m_sourceMACAddress(std::move(other.m_sourceMACAddress)),
    m_destinationMACAddress(std::move(other.m_destinationMACAddress)) {
    other.setParent(nullptr);
}

DataLinkHeader& DataLinkHeader::operator=(DataLinkHeader &&other) noexcept {
    if (this != &other) {
        m_sourceMACAddress = std::move(other.m_sourceMACAddress);
        m_destinationMACAddress = std::move(other.m_destinationMACAddress);
        setParent(other.parent());
        other.setParent(nullptr);
    }
    return *this;
}


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
