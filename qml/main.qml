import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0

import Cutefish.Dock 1.0
import MeuiKit 1.0 as Meui

Item {
    id: root
    visible: true

    property color borderColor: Meui.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.1) : Qt.rgba(0, 0, 0, 0.05)
    property real windowRadius: Settings.roundedWindowEnabled ? (Settings.direction === DockSettings.Left) ? root.width * 0.25 : root.height * 0.25
                                                              : 0
    property bool isHorizontal: Settings.direction !== DockSettings.Left

    DropArea {
        anchors.fill: parent
        enabled: true
    }

    Meui.WindowShadow {
        view: mainWindow
        geometry: Qt.rect(root.x, root.y, root.width, root.height)
        radius: _background.radius
    }

    Meui.WindowBlur {
        view: mainWindow
        geometry: Qt.rect(root.x, root.y, root.width, root.height)
        windowRadius: _background.radius
        enabled: true
    }

    // Background
    Rectangle {
        id: _background
        anchors.fill: parent
        radius: windowRadius
        color: Meui.Theme.backgroundColor
        opacity: Meui.Theme.darkMode ? 0.3 : 0.5

        Behavior on opacity {
            NumberAnimation {
                duration: 200
                easing.type: Easing.Linear
            }
        }

        Behavior on color {
            ColorAnimation {
                duration: 200
                easing.type: Easing.Linear
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        radius: windowRadius
        visible: windowRadius
        border.width: 1
        border.color: Qt.rgba(0, 0, 0, 0.5)
        antialiasing: true
        smooth: true
    }

    Rectangle {
        anchors.fill: parent
        anchors.margins: 1
        radius: windowRadius - 1
        visible: windowRadius
        color: "transparent"
        border.width: 1
        border.color: Qt.rgba(255, 255, 255, 0.3)
        antialiasing: true
        smooth: true
    }

    Meui.PopupTips {
        id: popupTips
        backgroundColor: Meui.Theme.backgroundColor
        backgroundOpacity: Meui.Theme.darkMode ? 0.3 : 0.4
    }

    GridLayout {
        id: mainLayout
        anchors.fill: parent
        flow: isHorizontal ? Grid.LeftToRight : Grid.TopToBottom
        columnSpacing: 0
        rowSpacing: 0

        ListView {
            id: appItemView
            orientation: isHorizontal ? Qt.Horizontal : Qt.Vertical
            snapMode: ListView.SnapToItem
            interactive: false
            model: appModel
            clip: true

            Layout.fillHeight: true
            Layout.fillWidth: true

            delegate: AppItem {
                id: appItemDelegate
                implicitWidth: isHorizontal ? appItemView.height : appItemView.width
                implicitHeight: isHorizontal ? appItemView.height : appItemView.width
            }

            moveDisplaced: Transition {
                NumberAnimation {
                    properties: "x, y"
                    duration: 300
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }

    Connections {
        target: Settings

        function onDirectionChanged() {
            popupTips.hide()
        }
    }
}
