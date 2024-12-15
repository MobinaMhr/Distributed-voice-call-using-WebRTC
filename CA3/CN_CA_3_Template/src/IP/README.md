# IP

The `IP` class template is designed to handle both IPv4 and IPv6 addresses in a network simulation or application. It provides functionalities for converting between string representations and numerical values, managing subnet masks and prefix lengths, and performing subnet calculations. The class supports both IP versions through template specialization.

### Utility functions

```cpp
uint32_t subnetMaskToValue(const QString &subnetMask) {
    QStringList parts = subnetMask.split('.');
    if (parts.size() != 4)
        throw std::invalid_argument(SUBNET_MASK_FORMAT_ERROR);
    uint32_t value = 0;
    for (int i = 0; i < 4; ++i)
        value |= (parts[i].toInt() & 0xFF) << (24 - 8 * i);
    return value;
}
```

The `subnetMaskToValue()` function out side of classes, converts a subnet mask string to a 32 bit integer value.

The input strign could be in the format of : `"255.255.255.0"`.

```cpp
QString valueToSubnetMask(uint32_t value) {
    QStringList parts;
    for (int i = 0; i < 4; ++i)
        parts << QString::number((value >> (24 - 8 * i)) & 0xFF);
    return parts.join('.');
}
```

The `valueToSubnetMask()` function out side of classes, converts a 32-bit integer value to a subbnet mask string.

```cpp
QByteArray bitwiseAnd(const QByteArray &array1, const QByteArray &array2) {
    if (array1.size() != array2.size()) {
        qWarning() << "Byte arrays must have the same size for bitwise operations.";
        return QByteArray();
    }
    QByteArray result(array1.size(), 0);
    for (int i = 0; i < array1.size(); ++i) {
        result[i] = array1[i] & array2[i];
    }
    return result;
}
```

The `bitwiseAnd()` function out side of classes, performs a bitwise AND operation on two byte arrays of the same size.

### AbstractIP

```cpp
bool AbstractIP::operator<(const AbstractIP& other) const {
    IPv4Ptr_t myIPV4 = this->toIPv4();
    IPv4Ptr_t theirIPV4 = other.toIPv4();
    return myIPV4->toValue() < theirIPV4->toValue();
}
```
This overloaded operator compares two IP addresses based on their numerical values.
It has not been used in the code yet, so I'm commenting it right now :D

### IPv4

This class has two constructors with two type of given iP in QString and utint32-t types.

These constructos initialze base class (`AbstractIP`).
after calling the parent constructor, it sets some private attributes.

```cpp
IP<UT::IPVersion::IPv4>::IP(const QString &ipString, const QString &subnetMask, QObject *parent) :
    AbstractIP(parent), m_ipValue(DEFAULT_IP_VALUE), m_subnetMask(DEFAULT_SUBNET_MASK_VALUE){

    QStringList parts = ipString.split('.');
    if (parts.size() != 4)
        throw std::invalid_argument("Invalid IPv4 format");
    for (int i = 0; i < 4; ++i)
        m_ipValue |= (parts[i].toInt() & 0xFF) << (24 - 8 * i);

    if (!subnetMask.isEmpty())
        setSubnetMask(subnetMask);
    else
        setSubnetMask(DEFAULT_SUBNET_MASK);
}
```

Then it parses the IP in string format. It splits the   `ipString` into parts using the dot (`.`) delimiter. If the number of parts is not equal to 4, it throws an `std::invalid_argument` exception with the message "Invalid IPv4 format".

Then it converts each part of the IP address to an integer and combines them into a **32-bit** value using bitwise operations.

If the subnetMask is not empty, it sets the subnet mask using the `setSubnetMask` method. Otherwise, it sets the subnet mask to the default value.

```cpp
IP<UT::IPVersion::IPv4>::IP(uint32_t ipValue, const QString &subnetMask, QObject *parent) :
    AbstractIP(parent), m_ipValue(ipValue), m_subnetMask(DEFAULT_SUBNET_MASK_VALUE){
    if (!subnetMask.isEmpty())
        setSubnetMask(subnetMask);
    else
        setSubnetMask(DEFAULT_SUBNET_MASK);
}
```

The different logis of the second construtor with the first one is that it doesn't iterate and already has the value in needed format.

It has some getters and setters as:

```cpp
QString IP<UT::IPVersion::IPv4>::toString() const{
    QStringList parts;
    for (int i = 0; i < 4; ++i)
        parts << QString::number((m_ipValue >> (24 - 8 * i)) & 0xFF);
    return parts.join('.');
}
```

The toString returns stringified IP address.
It iterates over the byte array and converts bytes to **Hexadecimal** format. It appends all results to the string and returns it.

```cpp
uint32_t IP<UT::IPVersion::IPv4>::toValue() const{
    return m_ipValue;
}
```

This method returns the 32-bit value of IP address.

```cpp
QString IP<UT::IPVersion::IPv4>::getSubnetMask() const {
    return valueToSubnetMask(m_subnetMask);
}
```

This method returns the QString format of subnet mask, by making a function call on `valueToSubnetMask()` method.

- A subnetwork, or subnet, is a logical subdivision of an IP network. The practice of dividing a network into two or more networks is called subnetting.

![subnet](https://github.com/MobinaMhr/Computer-Networks-Course-S2025/blob/main/CA3/assets/Subnetting_Concept-en.svg.png) 


```cpp
bool IP<UT::IPVersion::IPv4>::isInSubnet(const QString &otherIP) const {
    QHostAddress addr(otherIP);
    if (addr.protocol() != QAbstractSocket::IPv4Protocol) {
        return false;
    }
    uint32_t otherValue = addr.toIPv4Address();
    return (m_ipValue & m_subnetMask) == (otherValue & m_subnetMask);
}
```

The `isInSubnet()` method checks if an IP address is within the same subnet or not.

It converts `otherIp` which is it QString format to QHostAddress format.

Then checks if the address protocol is same with IPV4Protocol or not, if not it returns false, meaning it is not in the subnet.

If they are same, converts the address to IPv4 address and the conversion is valid.

Performs a bitwise and between `m_ipValue` and `m_subnetMask` and checks if it is the same with `otherValue` and the same `m_subnetMask`.

If they are equal, it returns true nad if not, returns false.

```cpp
IPv4Ptr_t IP<UT::IPVersion::IPv4>::toIPv4() const
{
    return QSharedPointer<IPv4_t>::create(new IPv4_t(m_ipValue, getSubnetMask()));
}
```

```cpp
IPv6Ptr_t IP<UT::IPVersion::IPv4>::toIPv6() const
{
    QByteArray ipv6Bytes(IPV6_Length_IN_BYTES, 0);
    ipv6Bytes[10] = static_cast<char>(0xFF);
    ipv6Bytes[11] = static_cast<char>(0xFF);
    ipv6Bytes[12] = static_cast<char>((m_ipValue >> 24) & 0xFF);
    ipv6Bytes[13] = static_cast<char>((m_ipValue >> 16) & 0xFF);
    ipv6Bytes[14] = static_cast<char>((m_ipValue >> 8) & 0xFF);
    ipv6Bytes[15] = static_cast<char>(m_ipValue & 0xFF);
    // Create and return an IPv6 instance
    return QSharedPointer<IPv6_t>::create(new IPv6_t(ipv6Bytes, IPV6_Length_IN_BITS));
}
```

The `toIPv4()` and `toIPv6()` methods in the IPv4 class return a created shared pointer with its attributes. this is coded for compatability of sending and receiving packets in both IP versions.

The `toIPv4()` returns a QShared pointer of its instance.

The `toIPv6()` returns a QShared pointer of a IPv6 instance with converted values.


### IPv6

The constructor calls the parent constructor and sets some private attributes.
Its constructor functionality is just like IP4 version.

This class has some constructors to cover many formats if input IP address in **QString**, **QByte array**.

They finaly call the parent constructor and set some private attributes.

These constructors have some functionality to handle more thant on IP address input format.

```cpp
IP<UT::IPVersion::IPv6>::IP(QObject *parent) :
    AbstractIP(parent), m_ipValue(IPV6_Length_IN_BYTES, IPV6_DEFAULT_FILL_VALUE),
    m_prefixLength(DEFAULT_IPV6_PREFIX_LENGTH){}
```

```cpp
IP<UT::IPVersion::IPv6>::IP(const QString &ipString, int prefixLength, QObject *parent) :
    AbstractIP(parent), m_ipValue(IPV6_Length_IN_BYTES, IPV6_DEFAULT_FILL_VALUE),
    m_prefixLength(prefixLength){
    QString modifiedString = ipString;
    modifiedString.remove(IPV6_DELIM);
    m_ipValue = QByteArray::fromHex(modifiedString.toUtf8());
    if (m_ipValue.size() != IPV6_Length_IN_BYTES)
        throw std::invalid_argument(IPV6_VALUE_ERROR);
}
```

```cpp
IP<UT::IPVersion::IPv6>::IP(const QByteArray &ipValue, int prefixLength, QObject *parent) :
    AbstractIP(parent), m_ipValue(ipValue), m_prefixLength(prefixLength){
    if (m_ipValue.size() != IPV6_Length_IN_BYTES)
        throw std::invalid_argument(IPV6_VALUE_ERROR);
}
```

This method converts the `m_ipValue` to a QString object.

```cpp
QString IP<UT::IPVersion::IPv6>::toString() const{
    QString result;
    for (int i = 0; i < m_ipValue.size(); i += 2) {
        if (i > 0) result += ':';
        result += QString::number((m_ipValue[i] << 8) | m_ipValue[i + 1], 16);
    }
    return result;
}
```

It iterates over the byte array and converts bytes to **Hexadecimal** format. It appends all results to the string and returns it.

```cpp
QByteArray IP<UT::IPVersion::IPv6>::toValue() const
{
    return m_ipValue;
}
```

This is a simple getter methos.

```cpp
int IP<UT::IPVersion::IPv6>::getPrefixLength() const
{
    return m_prefixLength;
}
```

This is a simple getter methos.

```cpp
void IP<UT::IPVersion::IPv6>::setPrefixLength(int prefixLength)
{
    if (prefixLength < 0 || prefixLength > IPV6_Length_IN_BITS)
        throw std::invalid_argument(INVALID_PREFIX_LENGTH_ERROR);
    m_prefixLength = prefixLength;
}
```

This method named `setPrefixLength()` sets the prefix length for the **IPv6 address**, which is used to determine the subnet mask. The prefix length defines the number of bits in the address that are used for the network portion, with the remaining bits used for the host portion.

It checks if the input is valid or not, if not, it throws and exception named `invalid_argument`.
Then, it acts like a simple setter.

```cpp
QPair<QString, QString> IP<UT::IPVersion::IPv6>::getSubnetRange() const
{
    QByteArray lowerBound = m_ipValue;
    QByteArray upperBound = m_ipValue;
    int hostBits = IPV6_Length_IN_BITS - m_prefixLength;

    for (int i = 15; i >= 0 && hostBits > 0; --i) {
        int clearBits = std::min(8, hostBits);
        upperBound[i] |= (0xFF >> (8 - clearBits));
        hostBits -= clearBits;
    }
    return { QHostAddress(lowerBound).toString(), QHostAddress(upperBound).toString() };
}
```
The `getSubnetRange()` method calculates and returns the range of IP addresses within the same subnet as the current IPv6 address (`m_ipValue`). The range is determined based on the subnet mask defined by the prefix length (`m_prefixLength`).

It first initializes the lower and upper bounds with `m_ipValue`.
Then, calculates the Host bits by subtracting the prefix length (`m_prefixLength`) from the total number of bits in an IPv6 address (`IPV6_Length_IN_BITS`).

Then it iterates over the bytes of the `upperBound` array from the end (least significant byte) to the beginning (most significant byte).

For each byte, it calculates the number of bits to clear, saved it `clearBits` variable, with the functionality as : `std::min(8, hostBits)`, and sets the corresponding bits in the upperBound array to : `(0xFF >> (8 - clearBits))`. 

Finaly it returns a QPair of two QStrings with the value of converted `lowerBound` and `upperBound` to QHostAddress object and then to QString.

```cpp
bool IP<UT::IPVersion::IPv6>::isInSubnet(const QString &otherIP) const
{
    QHostAddress addr(otherIP);
    if (addr.protocol() != QAbstractSocket::IPv6Protocol) {
        return false;
    }
    QString modifiedString = otherIP;
    modifiedString.remove(IPV6_DELIM);
    QByteArray otherValue = QByteArray::fromHex(modifiedString.toUtf8());
    QByteArray mask(IPV6_Length_IN_BYTES, IPV6_DEFAULT_FILL_VALUE);
    int hostBits = IPV6_Length_IN_BITS - m_prefixLength;

    for (int i = 15; i >= 0 && hostBits > 0; --i) {
        int clearBits = std::min(8, hostBits);
        mask[i] = ~((1 << clearBits) - 1);
        hostBits -= clearBits;
    }
    return bitwiseAnd(m_ipValue, mask) == bitwiseAnd(otherValue, mask);
}
```

This method checks whether a given IPv6 address (otherIP) is within the same subnet as the current IPv6 address (m_ipValue). It uses the subnet mask defined by the prefix length (m_prefixLength) to perform this check.

It first converts 
Then removes delimiters and converts to byte array structure.

Then creates a subnet mask and calculates the number of host bits by subtracting the prefix length from the total number of bits in an IPv6 address (`IPV6_Length_IN_BITS`).

It iterates over the bytes of the mask from the end (least significant byte) to the begining (most significant byte).

For each byte, it calculates the number of bits to clear (`clearBits`) and sets the corresponding bits in the mask to zero.

It gets the returned amout of perfomed a bitwise and operator between the current IPv6 address (`m_ipValue`) and the mask.

It calls the same function ot the `otherValue` and mask.

Finaly it compares theses result with each other. If they are equal, returns true and if not, returns false. 


```cpp
IPv6Ptr_t IP<UT::IPVersion::IPv6>::toIPv6() const {
    return QSharedPointer<IPv6_t>::create(new IPv6_t(m_ipValue, m_prefixLength));
}
```

```cpp
IPv4Ptr_t IP<UT::IPVersion::IPv6>::toIPv4() const {
    if (m_ipValue.startsWith(QByteArray::fromHex("00000000000000000000FFFF"))) {
        uint32_t ipv4Value = (static_cast<uint32_t>(static_cast<uint8_t>(m_ipValue[12])) << 24) |
                             (static_cast<uint32_t>(static_cast<uint8_t>(m_ipValue[13])) << 16) |
                             (static_cast<uint32_t>(static_cast<uint8_t>(m_ipValue[14])) << 8) |
                             static_cast<uint32_t>(static_cast<uint8_t>(m_ipValue[15]));
        return IPv4Ptr_t::create(ipv4Value);
    } else {
        throw std::invalid_argument("Not a mapped IPv4 address");
    }
}
```

The `toIPv6()` and `toIPv4()` methods in the IPv6 class return a created shared pointer with its attributes. this is coded for compatability of sending and receiving packets in both IP versions.

The `toIPv6()` returns a QShared pointer of its instance.

The `toIPv4()` returns a QShared pointer of a IPv6 instance with converted values.

# IPHeader