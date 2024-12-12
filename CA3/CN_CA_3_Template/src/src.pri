QT     += core
CONFIG += lrelease
CONFIG += no_keywords
CONFIG += c++20

INCLUDEPATH += $$PWD\Globals

SOURCES += \
    # $$PWD/DHCPServer/DHCPServer.cpp \
    $$PWD/DataGenerator/datagenerator.cpp \
    $$PWD/EventsCoordinator/EventsCoordinator.cpp \
    $$PWD/IP/IP.cpp \
    $$PWD/MACAddress/macAddress.cpp \
    $$PWD/MACAddress/macaddressgenerator.cpp \
    # $$PWD/MACAddress/testmacaddress.cpp \
    # $$PWD/MACAddress/testmacaddressgenerator.cpp \
    # $$PWD/PortBindingManager/PortBindingManager.cpp \
    # $$PWD/Port/Port.cpp \
    # $$PWD/TCPHeader/tcpheader.cpp \
    $$PWD/main.cpp

HEADERS += \
    # $$PWD/DHCPServer/DHCPServer.h \
    $$PWD/DataGenerator/datagenerator.h \
    $$PWD/EventsCoordinator/EventsCoordinator.h \
    $$PWD/Globals/Globals.h \
    $$PWD/IP/IP.h \
    $$PWD/MACAddress/macAddress.h \
    $$PWD/MACAddress/macaddressgenerator.h \
    # $$PWD/MACAddress/testmacaddress.h \
    # $$PWD/MACAddress/testmacaddressgenerator.h \
    # $$PWD/PortBindingManager/PortBindingManager.h \
    # $$PWD/Port/Port.h \
    # $$PWD/TCPHeader/tcpheader.h

# DISTFILES += \
#     $$PWD/MACAddress/README.md
