/*
 * Copyright (C) 2021 CutefishOS Team.
 *
 * Author:     rekols <revenmartin@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

    // Background
    Rectangle {
        id: _background
        anchors.fill: parent
        radius: windowRadius
        color: FishUI.Theme.darkMode ? "#828286" : "#F2F2F2"
        opacity: FishUI.Theme.darkMode ? 0.5 : 0.4
        border.width: 0

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

        DockItem {
            id: trashItem
            implicitWidth: isHorizontal ? root.height : root.width
            implicitHeight: isHorizontal ? root.height : root.width
            popupText: qsTr("Trash")
            enableActivateDot: false
            iconName: trash.count === 0 ? "user-trash-empty" : "user-trash-full"
            onClicked: trash.openTrash()
            onRightClicked: trashMenu.popup()

            FishUI.DesktopMenu {
                id: trashMenu

                MenuItem {
                    text: qsTr("Open")
                    onTriggered: trash.openTrash()
                }

                MenuItem {
                    text: qsTr("Empty Trash")
                    onTriggered: trash.emptyTrash()
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
