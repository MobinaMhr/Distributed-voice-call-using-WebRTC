QT += quick
QT += multimedia

SOURCES += \
        audioinput.cpp \
        audiooutput.cpp \
        audioprocessor.cpp \
        callmanager.cpp \
        main.cpp \
        webrtc.cpp

resources.files = main.qml 
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
    audioinput.h \
    audiooutput.h \
    audioprocessor.h \
    callmanager.h \
    webrtc.h


INCLUDEPATH += ./../../../libdatachannel/include
LIBS += -L./../../../libdatachannel/Windows/Mingw64 -ldatachannel.dll
LIBS += -LC:/Qt/Tools/OpenSSLv3/Win_x64/bin -lcrypto-3-x64 -lssl-3-x64
LIBS += -lws2_32
LIBS += -lssp

INCLUDEPATH += ./../../../opus/include
LIBS += -L./../../../opus/Windows/Mingw64 -lopus
