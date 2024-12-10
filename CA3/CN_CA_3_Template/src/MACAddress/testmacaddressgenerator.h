#ifndef TESTMACADDRESSGENERATOR_H
#define TESTMACADDRESSGENERATOR_H

#include <QObject>
#include "./macaddressgenerator.h"

class TestMacAddressGenerator : public QObject
{
    Q_OBJECT

public:
    explicit TestMacAddressGenerator(QObject *parent = nullptr);
    bool testGenerateUniqueAddresses();
    void runAllTests();

Q_SIGNALS:
};

#endif    // TESTMACADDRESSGENERATOR_H
