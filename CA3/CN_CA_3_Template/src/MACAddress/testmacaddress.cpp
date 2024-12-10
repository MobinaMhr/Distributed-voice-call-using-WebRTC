#include "testmacaddress.h"

TestMacAddress::TestMacAddress(QObject *parent) :
    QObject {parent}
{}

bool TestMacAddress::testConstructorUint64()
{
    try {
        uint64_t address = 0xAABBCCDDEEFF;
        MacAddress mac(address);
        return mac.toString() == "aa:bb:cc:dd:ee:ff";
    } catch (...) {
        return false;
    }
}
