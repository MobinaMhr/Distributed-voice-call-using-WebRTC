#ifndef TESTMACADDRESS_H
#define TESTMACADDRESS_H

#include <QObject>
#include "./macAddress.h"

class TestMacAddress : public QObject
{
    Q_OBJECT

public:
    explicit TestMacAddress(QObject *parent = nullptr);
    bool testConstructorUint64();
    bool testConstructorQString();
    bool testInvalidMacAddress();
    bool testToString();
    void runAllTests();

Q_SIGNALS:
};

#endif    // TESTMACADDRESS_H
