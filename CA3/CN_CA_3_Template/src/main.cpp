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

    QString configFilePath = "config.json";  // Provide the actual config file path.
    Network network(configFilePath);

    // Verify if the configuration file was read successfully.
    network.isConfigLoaded();

    qDebug() << "----------------------------------";

    return app.exec();
}

