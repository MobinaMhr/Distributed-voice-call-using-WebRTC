#ifndef DATALINKHEADER_H
#define DATALINKHEADER_H

#include <QObject>
#include "../MACAddress/macAddress.h"

class DataLinkHeader : public QObject {
    Q_OBJECT
public:
    explicit DataLinkHeader(const MacAddress &src, const MacAddress &dest,
                            QObject *parent = nullptr);
    MacAddress source() const;
    MacAddress destination() const;


    void setSourceMACAddress(const MacAddress &newSourceMACAddress);

    void setDestinationMACAddress(const MacAddress &newDestinationMACAddress);

private:
    MacAddress m_sourceMACAddress;
    MacAddress m_destinationMACAddress;
};

#endif // DATALINKHEADER_H
