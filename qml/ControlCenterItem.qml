import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0

import Cutefish.Dock 1.0
import MeuiKit 1.0 as Meui

StandardItem {
    id: controlItem

    Layout.preferredWidth: isHorizontal ? controlLayout.implicitWidth + root.largeSpacing * 2 : mainLayout.width * 0.7
    Layout.preferredHeight: isHorizontal ? mainLayout.height * 0.7 : controlLayout.implicitHeight + root.largeSpacing * 2
    Layout.rightMargin: isHorizontal ? root.windowRadius / 2 : 0
    Layout.bottomMargin: isHorizontal ? 0 : root.windowRadius / 2
    Layout.alignment: Qt.AlignCenter

    onClicked: {
        if (controlCenter.visible)
            controlCenter.visible = false
        else {
            // 先初始化，用户可能会通过Alt鼠标左键移动位置
            controlCenter.position = Qt.point(0, 0)

            controlCenter.visible = true
            controlCenter.position = Qt.point(mapToGlobal(0, 0).x, mapToGlobal(0, 0).y)
        }
    }

    GridLayout {
        id: controlLayout
        anchors.fill: parent
        anchors.leftMargin: isHorizontal ? root.smallSpacing : 0
        anchors.rightMargin: isHorizontal ? root.smallSpacing : 0
        anchors.topMargin: isHorizontal ? root.smallSpacing : 0
        anchors.bottomMargin: isHorizontal ? root.smallSpacing : 0
        columnSpacing: isHorizontal ? root.largeSpacing + root.smallSpacing : 0
        rowSpacing: isHorizontal ? 0 : root.largeSpacing + root.smallSpacing
        flow: isHorizontal ? Grid.LeftToRight : Grid.TopToBottom

        Image {
            id: wirelessIcon
            width: root.trayItemSize
            height: width
            sourceSize: Qt.size(width, height)
            source: network.wirelessIconName ? "qrc:/svg/" + (Meui.Theme.darkMode ? "dark/" : "light/") + network.wirelessIconName + ".svg" : ""
            asynchronous: true
            Layout.alignment: Qt.AlignCenter
            visible: network.enabled &&
                     network.wirelessEnabled &&
                     network.wirelessConnectionName !== "" &&
                     wirelessIcon.status === Image.Ready
        }

        Image {
            id: batteryIcon
            visible: battery.available && status === Image.Ready
            height: root.trayItemSize
            width: height + 6
            sourceSize: Qt.size(width, height)
            source: "qrc:/svg/" + (Meui.Theme.darkMode ? "dark/" : "light/") + battery.iconSource
            asynchronous: true
            Layout.alignment: Qt.AlignCenter
        }

        Image {
            id: volumeIcon
            visible: volume.isValid && status === Image.Ready
            source: "qrc:/svg/" + (Meui.Theme.darkMode ? "dark/" : "light/") + volume.iconName + ".svg"
            width: root.trayItemSize
            height: width
            sourceSize: Qt.size(width, height)
            asynchronous: true
            Layout.alignment: Qt.AlignCenter
        }

        Label {
            id: timeLabel
            Layout.alignment: Qt.AlignCenter
            font.pixelSize: isHorizontal ? controlLayout.height / 3 : controlLayout.width / 5

            Timer {
                interval: 1000
                repeat: true
                running: true
                triggeredOnStart: true
                onTriggered: {
                    timeLabel.text = new Date().toLocaleTimeString(Qt.locale(), Locale.ShortFormat)
                }
            }
        }
    }
}
