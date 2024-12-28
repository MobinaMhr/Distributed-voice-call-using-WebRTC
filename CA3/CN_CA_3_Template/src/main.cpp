#include <QCoreApplication>
#include <QDebug>
#include "./Network/network.h"
// #include "./MACAddress/testmacaddress.h"
// #include "./MACAddress/testmacaddressgenerator.h"

// void testMACAddress() {
//     qDebug() << "Testing MacAddress :";
//     TestMacAddress tm ;
//     tm.runAllTests();
//     qDebug() << "----------------------------------";
// }

// void testMacAddressGenerator(){
//     qDebug() << "Testing MacAddress Generator :";
//     TestMacAddressGenerator tmg ;
//     tmg.runAllTests();
//     qDebug() << "----------------------------------";
// }

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    QString configFilePath = "./../../assets/config.json";
    Network network(configFilePath);

    network.isConfigLoaded();

    qDebug() << "----------------------------------";

    return app.exec();
}

