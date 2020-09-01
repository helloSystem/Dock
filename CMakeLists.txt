cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME cyber-dock)
project(${PROJECT_NAME})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT Core X11Extras Quick QuickControls2)
find_package(Qt5 REQUIRED ${QT})
find_package(KF5WindowSystem REQUIRED)

set(SRCS
    src/applicationitem.h
    src/applicationmodel.cpp
    src/docksettings.cpp
    src/iconthemeimageprovider.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/processprovider.cpp
    src/popuptips.cpp
    src/xwindowinterface.cpp
)

set(RESOURCES
    resources.qrc
)

add_executable(${PROJECT_NAME} ${SRCS} ${DBUS_SRCS} ${RESOURCES})
target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::X11Extras
        Qt5::Quick
        Qt5::QuickControls2
        KF5::WindowSystem
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION /usr/bin)
