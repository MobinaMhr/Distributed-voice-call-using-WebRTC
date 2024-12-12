QT     += core
CONFIG += lrelease
CONFIG += no_keywords
CONFIG += c++20

INCLUDEPATH += $$PWD\Globals

SOURCES += \
    $$PWD/AutonomousSystem/autonomoussystem.cpp \
    $$PWD/DHCPServer/DHCPServer.cpp \
    $$PWD/DataGenerator/datagenerator.cpp \
    $$PWD/EventsCoordinator/EventsCoordinator.cpp \
    $$PWD/DataLinkHeader/datalinkheader.cpp \
    $$PWD/IP/IP.cpp \
    $$PWD/IP/ipheader.cpp \
    $$PWD/MACAddress/macAddress.cpp \
    $$PWD/MACAddress/macaddressgenerator.cpp \
    $$PWD/MACAddress/testmacaddress.cpp \
    $$PWD/MACAddress/testmacaddressgenerator.cpp \
    $$PWD/Node/node.cpp \
    $$PWD/PC/pc.cpp \
    $$PWD/PortBindingManager/PortBindingManager.cpp \
    $$PWD/Port/Port.cpp \
    $$PWD/Packet/Packet.cpp \
    # $$PWD/Router/router.cpp \
    $$PWD/TCPHeader/tcpheader.cpp \
    $$PWD/Topology/topologybuilder.cpp \
    $$PWD/Topology/topologycontroller.cpp \
    $$PWD/main.cpp

HEADERS += \
    $$PWD/AutonomousSystem/autonomoussystem.h \
    $$PWD/DHCPServer/DHCPServer.h \
    $$PWD/DataGenerator/datagenerator.h \
    $$PWD/EventsCoordinator/EventsCoordinator.h \
    $$PWD/DataLinkHeader/datalinkheader.h \
    $$PWD/Globals/Globals.h \
    $$PWD/IP/IP.h \
    $$PWD/IP/ipheader.h \
    $$PWD/MACAddress/macAddress.h \
    $$PWD/MACAddress/macaddressgenerator.h \
    $$PWD/MACAddress/testmacaddress.h \
    $$PWD/MACAddress/testmacaddressgenerator.h \
    $$PWD/Node/node.h \
    $$PWD/PC/pc.h \
    $$PWD/PortBindingManager/PortBindingManager.h \
    $$PWD/Port/Port.h \
    $$PWD/Packet/Packet.h \
    # $$PWD/Router/router.h \
    $$PWD/TCPHeader/tcpheader.h \
    $$PWD/Topology/topologybuilder.h \
    $$PWD/Topology/topologycontroller.h

# DISTFILES += \
#     $$PWD/MACAddress/README.md
