import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import MeuiKit 1.0 as Meui

Item {
    id: control

    property url source
    property real size: 24
    property string popupText

    signal leftButtonClicked
    signal rightButtonClicked

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled: control.visible ? true : false

        onClicked: {
            if (mouse.button === Qt.LeftButton)
                control.leftButtonClicked()
            else if (mouse.button === Qt.RightButton)
                control.rightButtonClicked()
        }
    }

    Rectangle {
        anchors.fill: parent
        anchors.margins: 1
        radius: parent.height * 0.2

        color: {
            if (mouseArea.containsMouse) {
                if (mouseArea.containsPress)
                    return (Meui.Theme.darkMode) ? Qt.rgba(255, 255, 255, 0.3) : Qt.rgba(0, 0, 0, 0.3)
                else
                    return (Meui.Theme.darkMode) ? Qt.rgba(255, 255, 255, 0.2) : Qt.rgba(0, 0, 0, 0.2)
            }

            return "transparent"
        }
    }

    Image {
        id: iconImage
        anchors.centerIn: parent
        width: parent.height * 0.8
        height: width
        sourceSize.width: width
        sourceSize.height: height
        source: control.source
        asynchronous: true
    }
}
