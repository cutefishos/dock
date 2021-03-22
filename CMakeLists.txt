cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME cutefish-dock)
project(${PROJECT_NAME})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 CONFIG REQUIRED Widgets DBus X11Extras Concurrent Svg LinguistTools QuickControls2)

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
    src/popupwindow.cpp

    src/appearance.cpp
    src/fakewindow.cpp
    src/iconitem.cpp
    src/managedtexturenode.cpp

    src/systemtray/statusnotifieritemjob.cpp
    src/systemtray/statusnotifieritemsource.cpp
    src/systemtray/systemtraytypes.cpp
    src/systemtray/systemtraytypedefs.h
    src/systemtray/systemtraymodel.cpp
    src/systemtray/statusnotifierwatcher.cpp
)

set(RESOURCES
    resources.qrc
)

set(statusnotifierwatcher_xml src/systemtray/org.kde.StatusNotifierWatcher.xml)
qt5_add_dbus_interface(SRCS ${statusnotifierwatcher_xml} statusnotifierwatcher_interface)
qt5_add_dbus_interface(SRCS src/systemtray/org.freedesktop.DBus.Properties.xml dbusproperties)

set(statusnotifieritem_xml src/systemtray/org.kde.StatusNotifierItem.xml)
set_source_files_properties(${statusnotifieritem_xml} PROPERTIES
   NO_NAMESPACE false
   INCLUDE "src/systemtray/systemtraytypes.h"
   CLASSNAME OrgKdeStatusNotifierItem
)
qt5_add_dbus_interface(SRCS ${statusnotifieritem_xml} statusnotifieritem_interface)

qt5_add_dbus_adaptor(SRCS src/systemtray/org.kde.StatusNotifierWatcher.xml
                     src/systemtray/statusnotifierwatcher.h StatusNotifierWatcher)

add_executable(${PROJECT_NAME} ${SRCS} ${DBUS_SRCS} ${RESOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Core
        Qt5::Widgets
        Qt5::Quick
        Qt5::QuickControls2
        Qt5::X11Extras
        Qt5::Concurrent
        Qt5::DBus
        Qt5::Svg

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
