cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME cyber-dock)
project(${PROJECT_NAME})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT Core Widgets Quick QuickControls2 X11Extras)
find_package(Qt5 REQUIRED ${QT})
find_package(KF5WindowSystem REQUIRED)

# test
find_package(KF5Config)
find_package(KF5Service)

set(SRCS
    src/applicationitem.h
    src/applicationmodel.cpp
    src/docksettings.cpp
    src/iconthemeimageprovider.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/processprovider.cpp
    src/popuptips.cpp
    src/trashmanager.cpp
    src/utils.cpp
    src/xwindowinterface.cpp
    src/waylandinterface.cpp
)

set(RESOURCES
    resources.qrc
)

add_executable(${PROJECT_NAME} ${SRCS} ${DBUS_SRCS} ${RESOURCES})
target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::Widgets
        Qt5::Quick
        Qt5::QuickControls2
        Qt5::X11Extras

        KF5::WindowSystem
        KF5::ConfigCore
        KF5::ConfigGui
        KF5::Service
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION /usr/bin)
