#include <QCoreApplication>
#include <QDebug>
#include "./MACAddress/testmacaddress.h"
#include "./MACAddress/testmacaddressgenerator.h"

void testMACAddress() {
    qDebug() << "Testing MacAddress :";
    TestMacAddress tm ;
    tm.runAllTests();
    qDebug() << "----------------------------------";
}

void testMacAddressGenerator(){
    qDebug() << "Testing MacAddress Generator :";
    TestMacAddressGenerator tmg ;
    tmg.runAllTests();
    qDebug() << "----------------------------------";
}

int
main(int argv, char* argc[])
{
    QCoreApplication app(argv, argc);

    testMACAddress();
    testMacAddressGenerator();

    return app.exec();
}
