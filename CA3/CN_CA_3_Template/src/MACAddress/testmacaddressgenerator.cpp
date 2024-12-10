#include "testmacaddressgenerator.h"

TestMacAddressGenerator::TestMacAddressGenerator(QObject *parent) :
    QObject {parent}
{}

bool TestMacAddressGenerator::testGenerateUniqueAddresses()
{
    try {
        MacAddressGenerator generator;
        QSet<QString> generatedStrings;
        for (int i = 0; i < 100; ++i) {
            MacAddress mac = generator.generateMacAddress();
            QString macString = mac.toString();
            if (generatedStrings.contains(macString))
                return false;
            generatedStrings.insert(macString);
        }
        return true;
    } catch (...) {
        return false;
    }
}
