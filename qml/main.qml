import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0

import Cyber.NetworkManagement 1.0 as NM
import Cutefish.Dock 1.0
import MeuiKit 1.0 as Meui

Item {
    id: root
    visible: true

    property color borderColor: Meui.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.1) : Qt.rgba(0, 0, 0, 0.05)
    property real windowRadius: (Settings.direction === DockSettings.Left) ? root.width * 0.25 : root.height * 0.25
    property bool isHorizontal: Settings.direction !== DockSettings.Left
    property var appViewLength: isHorizontal ? appItemView.width : appItemView.height
    property var appViewHeight: isHorizontal ? appItemView.height : appItemView.width
    property var trayItemSize: 16
    property var iconSize: 0

    DropArea {
        anchors.fill: parent
        enabled: true
    }

    Timer {
        id: calcIconSizeTimer
        repeat: false
        running: false
        interval: 10
        onTriggered: calcIconSize()
    }

    function calcIconSize() {
        const appCount = appItemView.count
        const size = (appViewLength - (appViewLength % appCount)) / appCount
        const rootHeight = isHorizontal ? root.height : root.width
        const calcSize = size >= rootHeight ? rootHeight : Math.min(size, rootHeight)
        root.iconSize = calcSize
    }

    function delayCalcIconSize() {
        if (calcIconSizeTimer.running)
            calcIconSizeTimer.stop()

        calcIconSizeTimer.interval = 100
        calcIconSizeTimer.restart()
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
        border.width: 1
        border.color: Qt.rgba(0, 0, 0, 0.5)
        antialiasing: true
        smooth: true
    }

    Rectangle {
        anchors.fill: parent
        anchors.margins: 1
        radius: windowRadius - 1
        color: "transparent"
        border.width: 1
        border.color: Qt.rgba(255, 255, 255, 0.2)
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

            onCountChanged: root.delayCalcIconSize()

            delegate: AppItem {
                id: appItemDelegate
                implicitWidth: isHorizontal ? root.iconSize : ListView.view.width
                implicitHeight: isHorizontal ? ListView.view.height : root.iconSize

//                Behavior on implicitWidth {
//                    NumberAnimation {
//                        from: root.iconSize
//                        easing.type: Easing.InOutQuad
//                        duration: isHorizontal ? 250 : 0
//                    }
//                }

//                Behavior on implicitHeight {
//                    NumberAnimation {
//                        from: root.iconSize
//                        easing.type: Easing.InOutQuad
//                        duration: !isHorizontal ? 250 : 0
//                    }
//                }
            }

            moveDisplaced: Transition {
                NumberAnimation {
                    properties: "x, y"
                    duration: 300
                    easing.type: Easing.InOutQuad
                }
            }
        }

        ListView {
            id: trayView

            property var itemWidth: isHorizontal ? root.trayItemSize + Meui.Units.largeSpacing * 2 : mainLayout.width * 0.7
            property var itemHeight: isHorizontal ? mainLayout.height * 0.7 : root.trayItemSize + Meui.Units.largeSpacing * 2

            Layout.preferredWidth: isHorizontal ? itemWidth * count + count * trayView.spacing : mainLayout.width * 0.7
            Layout.preferredHeight: isHorizontal ? mainLayout.height * 0.7 : itemHeight * count + count * trayView.spacing
            Layout.alignment: Qt.AlignCenter

            orientation: isHorizontal ? Qt.Horizontal : Qt.Vertical
            layoutDirection: Qt.RightToLeft
            interactive: false
            model: SystemTrayModel { id: trayModel }
            spacing: Meui.Units.smallSpacing / 2
            clip: true

            onCountChanged: delayCalcIconSize()

            delegate: StandardItem {
                height: trayView.itemHeight
                width: trayView.itemWidth

                IconItem {
                    id: iconItem
                    anchors.centerIn: parent
                    width: root.trayItemSize
                    height: root.trayItemSize
                    source: model.icon ? model.icon : model.iconName
                }

                onClicked: trayModel.leftButtonClick(id)
                onRightClicked: trayModel.rightButtonClick(id)
                popupText: toolTip ? toolTip : title
            }
        }

        Item {
            width: Meui.Units.smallSpacing
        }

        ControlCenterItem {
            onWidthChanged: delayCalcIconSize()
            onHeightChanged: delayCalcIconSize()
        }
    }

    ControlDialog {
        id: controlCenter
    }

    Volume {
        id: volume
    }

    Battery {
        id: battery
    }

    NM.ConnectionIcon {
        id: connectionIconProvider
    }

    NM.Network {
        id: network
    }

    Connections {
        target: Settings

        function onDirectionChanged() {
            popupTips.hide()
        }
    }

    Connections {
        target: mainWindow

        function onResizingFished() {
            root.calcIconSize()
        }

        function onIconSizeChanged() {
            root.calcIconSize()
        }

        function onPositionChanged() {
            root.delayCalcIconSize()
        }
    }
}
