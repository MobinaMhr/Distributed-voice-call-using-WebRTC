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

bool TestMacAddress::testConstructorQString()
{
    try {
        QString address = "aa:bb:cc:dd:ee:ff";
        MacAddress mac(address);
        return mac.toString() == address;
    } catch (...) {
        return false;
    }
}

bool TestMacAddress::testInvalidMacAddress()
{
    try {
        QString invalidAddress = "zz:zz:zz:zz:zz:zz";
        MacAddress mac(invalidAddress);
        return false;
    } catch (const std::invalid_argument&) {
        return true;
    } catch (...) {
        return false;
    }
}

bool TestMacAddress::testToString()
{
    try {
        MacAddress mac(0xAABBCCDDEEFF);
        return mac.toString() == "aa:bb:cc:dd:ee:ff";
    } catch (...) {
        return false;
    }
}
