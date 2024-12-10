#include <QCoreApplication>
#include <QDebug>
#include "./MACAddress/testmacaddress.h"

void testMACAddress() {
    qDebug() << "Testing MacAddress :";
    TestMacAddress tm ;
    tm.runAllTests();
    qDebug() << "----------------------------------";
}

int
main(int argv, char* argc[])
{
    QCoreApplication app(argv, argc);

    testMACAddress();

    return app.exec();
}
