cmake_minimum_required(VERSION 3.16)

project(WebRTC_Call VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)


qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appWebRTC_Call
    main.cpp
)

qt_add_qml_module(appWebRTC_Call
    URI WebRTC_Call
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES callmanager.h callmanager.cpp
        SOURCES audioinput.h audioinput.cpp
        SOURCES audioinput.h audioinput.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appWebRTC_Call PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appWebRTC_Call
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appWebRTC_Call
    PRIVATE Qt6::Quick
    PRIVATE Qt6::Multimedia
)

include(GNUInstallDirs)
install(TARGETS appWebRTC_Call
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
