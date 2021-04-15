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
    property real windowRadius: isHorizontal ? root.height * 0.35 : root.width * 0.35

    DropArea {
        anchors.fill: parent
        enabled: true
    }

    DockBackground {
        anchors.fill: parent
        radius: root.windowRadius
        opacity: FishUI.Theme.darkMode ? 0.3 : 0.4
        color: FishUI.Theme.backgroundColor

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

    FishUI.WindowShadow {
        view: mainWindow
        geometry: Qt.rect(root.x, root.y, root.width, root.height)
        strength: 1
        radius: root.windowRadius
    }

    FishUI.WindowBlur {
        view: mainWindow
        geometry: Qt.rect(root.x, root.y, root.width, root.height)
        windowRadius: root.windowRadius
        enabled: true
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
