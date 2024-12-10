# Packet

This class mostly contains of getters and setters for Packet. 

Getter list:
```cpp
    UT::PacketType packetType() const;
    UT::PacketControlType controlType() const;
    QStringList path() const;
    quint32 sequenceNumber() const;
    quint32 waitingCycles() const;
    quint32 totalCycles() const;
    QString destinationIP() const;
    QByteArray payload() const;
    const DataLinkHeader& dataLinkHeader() const;
    const TCPHeader& tcpHeader() const;
```

Setter list:
```cpp
    void setPacketType(UT::PacketType packetType);
    void setControlType(UT::PacketControlType controlType);
    void setSequenceNumber(quint32 seqNumber);
    void setDestinationIP(QString destIP);
    void setPayload(const QByteArray &payload);
    void setDataLinkHeader(const DataLinkHeader &dataLinkHeader);
    void setTcpHeader(const TCPHeader &tcpHeader);
```

It also has some methods for increasing the waitingCycle and totalCycle by 1.

The `updateRoute()` method will add the ipAddress to the list of addresses.