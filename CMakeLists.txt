cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME cutefish-dock)
project(${PROJECT_NAME})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT Core Widgets Concurrent Quick QuickControls2 X11Extras DBus LinguistTools)
find_package(Qt5 REQUIRED ${QT})
find_package(KF5WindowSystem REQUIRED)
find_package(dbusmenu-qt5 REQUIRED)
find_package(MeuiKit REQUIRED)

set(SRCS
    src/applicationitem.h
    src/applicationmodel.cpp
    src/battery.cpp
    src/brightness.cpp
    src/controlcenterdialog.cpp
    src/docksettings.cpp
    src/iconthemeimageprovider.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/systemappmonitor.cpp
    src/systemappitem.cpp
    src/processprovider.cpp
    src/trashmanager.cpp
    src/volumemanager.cpp
    src/utils.cpp
    src/xwindowinterface.cpp

    src/appearance.cpp
    src/fakewindow.cpp

    src/statusnotifier/dbustypes.cpp
    src/statusnotifier/sniasync.cpp
    src/statusnotifier/statusnotifieriteminterface.cpp
    src/statusnotifier/statusnotifiermodel.cpp
    src/statusnotifier/statusnotifierwatcher.cpp
    src/statusnotifier/statusnotifieritemsource.cpp
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
        Qt5::Concurrent
        Qt5::DBus

        MeuiKit

        KF5::WindowSystem
        dbusmenu-qt5
)

file(GLOB TS_FILES translations/*.ts)
qt5_create_translation(QM_FILES ${TS_FILES})
add_custom_target(translations DEPENDS ${QM_FILES} SOURCES ${TS_FILES})
add_dependencies(${PROJECT_NAME} translations)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION /usr/bin)
install(FILES ${QM_FILES} DESTINATION /usr/share/${PROJECT_NAME}/translations)
