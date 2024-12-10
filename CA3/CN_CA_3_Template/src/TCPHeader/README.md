# TCPHeader

The `TCPHeader` class represents the header of a TCP (Transmission Control Protocol) packet. It encapsulates various fields such as source and destination ports, sequence and acknowledgment numbers, data offset, flags, window size, checksum, and urgent pointer. The class also supports copying, moving, and assigning instances, ensuring efficient resource management. It inherits from QObject, making it suitable for use in Qt applications.


The copy constructor creates a copy of an existing TCPHeader object.
```cpp
TCPHeader::TCPHeader(const TCPHeader &other) : QObject(other.parent()),
    m_sourcePort(other.m_sourcePort), m_destPort(other.m_destPort),
    m_sequenceNumber(other.m_sequenceNumber), m_acknowledgmentNumber(other.m_acknowledgmentNumber),
    m_dataOffset(other.m_dataOffset), m_flags(other.m_flags), m_windowSize(other.m_windowSize),
    m_checksum(other.m_checksum), m_urgentPointer(other.m_urgentPointer) {}
```


Cssignment Operator assigns the values from one TCPHeader object to another.
```cpp
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
```

Move Constructor moves the resources from one TCPHeader object to another.
```cpp
TCPHeader::TCPHeader(TCPHeader &&other) noexcept : QObject(other.parent()),
    m_sourcePort(other.m_sourcePort), m_destPort(other.m_destPort),
    m_sequenceNumber(other.m_sequenceNumber),
    m_acknowledgmentNumber(other.m_acknowledgmentNumber),
    m_dataOffset(other.m_dataOffset), m_flags(other.m_flags), m_windowSize(other.m_windowSize),
    m_checksum(other.m_checksum), m_urgentPointer(other.m_urgentPointer) {
    other.setParent(nullptr);
}
```

Move assignment operator moves the resources from one TCPHeader object to another.
```cpp
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
```

It has some getter and setter methods:
```cpp
uint16_t TCPHeader::sourcePort() const {
    return this->m_sourcePort;
}

uint16_t TCPHeader::destinationPort() const {
    return this->m_destPort;
}

uint32_t TCPHeader::sequenceNumber() const {
    return this->m_sequenceNumber;
}

uint32_t TCPHeader::acknowledgmentNumber() const {
    return this->m_acknowledgmentNumber;
}

uint8_t TCPHeader::dataOffset() const {
    return this->m_dataOffset;
}

uint8_t TCPHeader::flags() const {
    return this->m_flags;
}

uint16_t TCPHeader::windowSize() const {
    return this->m_windowSize;
}

uint16_t TCPHeader::checksum() const {
    return this->m_checksum;
}

uint16_t TCPHeader::urgentPointer() const {
    return this->m_urgentPointer;
}
```
```cpp
void TCPHeader::setSourcePort(uint16_t port) {
    this->m_sourcePort = port;
}

void TCPHeader::setDestPort(uint16_t port) {
    this->m_destPort = port;
}

void TCPHeader::setSequenceNumber(uint32_t sequenceNumber) {
    this->m_sequenceNumber = sequenceNumber;
}

void TCPHeader::setAcknowledgmentNumber(uint32_t acknowledgmentNumber) {
    this->m_acknowledgmentNumber = acknowledgmentNumber;
}

void TCPHeader::setDataOffset(uint8_t dataOffset) {
    this->m_dataOffset = dataOffset;
}

void TCPHeader::setFlags(uint8_t flags) {
    this->m_flags = flags;
}

void TCPHeader::setWindowSize(uint16_t windowSize) {
    this->m_windowSize = windowSize;
}

void TCPHeader::setChecksum(uint16_t checksum) {
    this->m_checksum = checksum;
}

void TCPHeader::setUrgentPointer(uint16_t urgentPointer) {
    this->m_urgentPointer = urgentPointer;
}
```