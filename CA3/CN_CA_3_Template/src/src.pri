QT     += core
CONFIG += lrelease
CONFIG += no_keywords
CONFIG += c++20

INCLUDEPATH += $$PWD\Globals

SOURCES += \
    $$PWD/IP/IP.cpp \
    $$PWD/IP/ipheader.cpp \
    $$PWD/TCPHeader/tcpheader.cpp \
    $$PWD/MACAddress/macAddress.cpp \
    $$PWD/DataLinkHeader/datalinkheader.cpp \
    $$PWD/Packet/Packet.cpp \
    $$PWD/Port/Port.cpp \
    $$PWD/Node/node.cpp \
    $$PWD/PC/pc.cpp \
    $$PWD/Router/routingtable.cpp \
    $$PWD/Router/router.cpp \
    $$PWD/PortBindingManager/PortBindingManager.cpp \
    $$PWD/MACAddress/macaddressgenerator.cpp \
    $$PWD/DataGenerator/datagenerator.cpp \
    $$PWD/EventsCoordinator/EventsCoordinator.cpp \
    $$PWD/Topology/topologybuilder.cpp \
    $$PWD/Topology/topologycontroller.cpp \
    # $$PWD/DHCPServer/DHCPServer.cpp \
    $$PWD/AutonomousSystem/autonomoussystem.cpp \
    # $$PWD/MACAddress/testmacaddress.cpp \
    # $$PWD/MACAddress/testmacaddressgenerator.cpp \
    $$PWD/main.cpp

HEADERS += \
    $$PWD/Globals/Globals.h \
    $$PWD/IP/IP.h \
    $$PWD/IP/ipheader.h \
    $$PWD/TCPHeader/tcpheader.h \
    $$PWD/MACAddress/macAddress.h \
    $$PWD/DataLinkHeader/datalinkheader.h \
    $$PWD/Packet/Packet.h \
    $$PWD/Port/Port.h \
    $$PWD/Node/node.h \
    $$PWD/PC/pc.h \
    $$PWD/Router/routingtable.h \
    $$PWD/Router/router.h \
    $$PWD/PortBindingManager/PortBindingManager.h \
    $$PWD/MACAddress/macaddressgenerator.h \
    $$PWD/DataGenerator/datagenerator.h \
    $$PWD/EventsCoordinator/EventsCoordinator.h \
    $$PWD/Topology/topologybuilder.h \
    $$PWD/Topology/topologycontroller.h \
    # $$PWD/DHCPServer/DHCPServer.h \
    $$PWD/AutonomousSystem/autonomoussystem.h
    # $$PWD/MACAddress/testmacaddress.h \
    # $$PWD/MACAddress/testmacaddressgenerator.h
