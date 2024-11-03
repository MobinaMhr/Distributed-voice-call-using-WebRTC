QT += quick
QT += multimedia
QT += websockets

SOURCES += \
        audio/audioinput.cpp \
        audio/audiooutput.cpp \
        audio/audioprocessor.cpp \
        main.cpp \
        network/socket.cpp \
        network/webrtc.cpp \
        src/callmanager.cpp

resources.files = ui/main.qml
resources.prefix = /$${TARGET}
RESOURCES += resources

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    audio/audioinput.h \
    audio/audiooutput.h \
    audio/audioprocessor.h \
    network/socket.h \
    network/webrtc.h \
    src/callmanager.h \

INCLUDEPATH += C:/Users/USER/Desktop/Github/libraries/libdatachannel/include
LIBS += -LC:/Users/USER/Desktop/Github/libraries/libdatachannel/Windows/Mingw64 -ldatachannel
LIBS += -LC:/Qt/Tools/OpenSSLv3/Win_x64/bin -lcrypto-3-x64 -lssl-3-x64
LIBS += -lws2_32
LIBS += -lssp

INCLUDEPATH += C:/Users/USER/Desktop/Github/libraries/opus/include
LIBS += -LC:/Users/USER/Desktop/Github/libraries/opus/Windows/Mingw64 -lopus

LIBS += -lssp

DISTFILES += \
    ui/Main.qml

