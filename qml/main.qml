import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0

import Cutefish.Dock 1.0
import FishUI 1.0 as FishUI

Item {
    id: root
    visible: true

    property bool isHorizontal: Settings.direction === DockSettings.Bottom
    property real windowRadius: isHorizontal ? root.height * 0.3 : root.width * 0.3

    DropArea {
        anchors.fill: parent
        enabled: true
    }

    FishUI.WindowShadow {
        view: mainWindow
        geometry: Qt.rect(root.x, root.y, root.width, root.height)
        strength: 0.5
        radius: _background.radius
    }

    FishUI.WindowBlur {
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
        color: FishUI.Theme.backgroundColor
        opacity: FishUI.Theme.darkMode ? 0.3 : 0.4

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
        border.color: Qt.rgba(0, 0, 0, 0.2)
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
        border.color: Qt.rgba(255, 255, 255, 0.2)
        antialiasing: true
        smooth: true
    }

    FishUI.PopupTips {
        id: popupTips
        backgroundColor: FishUI.Theme.backgroundColor
        backgroundOpacity: FishUI.Theme.darkMode ? 0.3 : 0.4
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
