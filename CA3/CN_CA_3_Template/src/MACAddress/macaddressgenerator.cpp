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
