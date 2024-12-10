# MACAddress
This class 

The `toString()` method 
```cpp
QString MacAddress::toString() const
{
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(12) << m_address;
    std::string hexString = ss.str();
    QString macString;
    for (size_t i = 0; i < hexString.size(); i += 2) {
        macString.append(QString::fromStdString(hexString.substr(i, 2)));
        if (i < hexString.size() - 2) {
            macString.append(":");
        }
    }
    return macString;
}
```

The `validate()` method ensures the address is within the valid range for MAC address which means it should be less than or equal to `MAX_MAC_ADDRESS` valued by `0xFFFFFFFFFFFF`.
```cpp
bool MacAddress::validate() const
{
    return m_address <= MAX_MAC_ADDRESS;
}
```

The `parseAddress()` is implemented as:
```cpp
uint64_t MacAddress::parseAddress(const QString &address) const
{
    QRegularExpression re("([0-9A-Fa-f]{2}[:\\-]?){6}");
    QRegularExpressionMatch match = re.match(address);

    if (!match.hasMatch())
        throw std::invalid_argument("Invalid MAC address format");

    QString sanitized = address;
    sanitized.remove(':').remove('-');
    bool ok; uint64_t mac = sanitized.toULongLong(&ok, 16);
    if (!ok)
        throw std::invalid_argument("Failed to parse MAC address");
    return mac;
}

```
It ensures the Mac address is in a valid format.
It creates a pattern matching six groups of two hexadecimal digits. They may optionally be separated by colons (`:`) or hyphens (`-`).
After this, it tries to match the input address against the created regular expression.
If the input address has no valid match, it throws an exception.

After these steps, it removes any colons or hyphens from the input address and creates a sanitized string of hexadecimal digits.
Then convert the sanitized string to a `uint64_t` value using base 16 or hexadecimal. If the conversion process fails, an exception is thrown.
The checked Mac address is returned.

# MACAddressGenerator
This class handles the creation of unique MAC addresses.
This class has a public method named `generateMacAddress()` implemented as:

```cpp
MacAddress MacAddressGenerator::generateMacAddress()
{
    uint64_t newAddress = generateUniqueAddress();
    generatedAddresses.insert(newAddress);
    return MacAddress(newAddress);
}
```
It calls the private method called `generateUniqueAddress`:
After that it inserts the new generated address to the private field set named `generatedAddresses`.
After all, it returns the output of MACAddress class, giving the generated address.

The `generateUniqueAddress` method implemented as:

```cpp
uint64_t MacAddressGenerator::generateUniqueAddress()
{
    uint64_t address;
    do { address = rng() & 0xFFFFFFFFFFFF;
    } while (generatedAddresses.contains(address));
    return address;
}
```

This method produces a random 48-bit MAC address. 
Then, it applies a bitwise and (&) to the maximum mac address, which is `0xFFFFFFFFFFFF`.
After the address is prepared, it checks whether it is unique. If it is not unique, the whole process starts again.
After generating a random unique Mac address, it returns it.

