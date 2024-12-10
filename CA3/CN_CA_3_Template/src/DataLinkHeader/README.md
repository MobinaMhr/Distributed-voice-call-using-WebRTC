# DataLinkHeader

The `DataLinkHeader` class is designed to represent the data link layer header in a network packet. It encapsulates the source and destination MAC addresses and provides functionality for copying, moving, and assigning instances of the class.

It has Copy Assignment Operator and move constructor as:

Copy Constructor creates a copy of an existing DataLinkHeader object.
```cpp
DataLinkHeader::DataLinkHeader(const DataLinkHeader &other) : QObject(other.parent()),
    m_sourceMACAddress(other.m_sourceMACAddress),
    m_destinationMACAddress(other.m_destinationMACAddress) {}

```

Copy Assignment Operator assigns the values from one DataLinkHeader object to another.
```cpp
DataLinkHeader& DataLinkHeader::operator=(const DataLinkHeader &other) {
    if (this != &other) {
        m_sourceMACAddress = other.m_sourceMACAddress;
        m_destinationMACAddress = other.m_destinationMACAddress;
        setParent(other.parent());
    }
    return *this;
}

```

Move constructore Moves the resources from one DataLinkHeader object to another.
```cpp
DataLinkHeader::DataLinkHeader(DataLinkHeader &&other) noexcept : QObject(other.parent()),
    m_sourceMACAddress(std::move(other.m_sourceMACAddress)),
    m_destinationMACAddress(std::move(other.m_destinationMACAddress)) {
    other.setParent(nullptr);
}
```

Move Assignment Operator moves the resources from one DataLinkHeader object to another.

```cpp
DataLinkHeader& DataLinkHeader::operator=(DataLinkHeader &&other) noexcept {
    if (this != &other) {
        m_sourceMACAddress = std::move(other.m_sourceMACAddress);
        m_destinationMACAddress = std::move(other.m_destinationMACAddress);
        setParent(other.parent());
        other.setParent(nullptr);
    }
    return *this;
}
```

After this, it has some getters and setters for source MAC address and destination MAC address.
