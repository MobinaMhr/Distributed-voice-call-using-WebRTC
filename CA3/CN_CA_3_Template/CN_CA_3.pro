TARGET = cnca3d
APPNAME = cnca3
VERSION = 1.0.0
BUNDLE_VERSION = 6

# QMAKE_LFLAGS += -fuse-ld=lld

# QT += core ## Added by me



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


DISTFILES += \
    .clang-format \
    .clang-tidy \
    .gitignore \
    assets/config.json


include($$PWD/src/src.pri)
