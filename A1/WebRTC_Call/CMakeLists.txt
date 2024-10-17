cmake_minimum_required(VERSION 3.16)
project(WebRTC_Call VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick Core Gui Multimedia)

# Diagnostic output for Qt6 Multimedia
if(TARGET Qt6::Multimedia)
    message(STATUS "Qt6 Multimedia found")
else()
    message(FATAL_ERROR "Qt6 Multimedia not found")
endif()

set(LIBDATACHANNEL_DIR "E:/UT_CN/Projects/libdatachannel/Windows/Mingw64")

include_directories("${LIBDATACHANNEL_DIR}/include")

link_directories("${LIBDATACHANNEL_DIR}/lib")

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appWebRTC_Call
    main.cpp
)

qt_add_qml_module(appWebRTC_Call
    URI WebRTC_Call
    VERSION 1.0
    QML_FILES Main.qml
    SOURCES
        callmanager.cpp callmanager.h
        webrtc.cpp webrtc.h
        audioinput.cpp audioinput.h
    DEPENDENCIES
        Qt6::Multimedia
)

set_target_properties(appWebRTC_Call PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appWebRTC_Call
    PRIVATE Qt6::Quick
    PRIVATE Qt6::Core
    PRIVATE Qt6::Gui
    PRIVATE Qt6::Multimedia
    PRIVATE "${LIBDATACHANNEL_DIR}/lib/libdatachannel.a"
    PRIVATE ${OPUS_LIB}
)

include(GNUInstallDirs)
install(TARGETS appWebRTC_Call
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
