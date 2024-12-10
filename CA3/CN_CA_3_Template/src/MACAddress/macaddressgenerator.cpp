#include "macaddressgenerator.h"

MacAddressGenerator::MacAddressGenerator(QObject *parent) :
    QObject {parent}
{}

MacAddress MacAddressGenerator::generateMacAddress()
{
    uint64_t newAddress = generateUniqueAddress();
    generatedAddresses.insert(newAddress);
    return MacAddress(newAddress);
}

uint64_t MacAddressGenerator::generateUniqueAddress()
{
    uint64_t address;
    do { address = rng() & 0xFFFFFFFFFFFF;
    } while (generatedAddresses.contains(address));
    return address;
}
