import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0
import MeuiKit 1.0 as Meui

Item {
    id: control

    property bool checked: false
    property alias icon: _image.source
    property alias label: _titleLabel.text
    property alias text: _label.text

    signal clicked

    property var hoverColor: Meui.Theme.darkMode ? Qt.lighter(Meui.Theme.secondBackgroundColor, 2)
                                                 : Qt.darker(Meui.Theme.secondBackgroundColor, 1.3)
    property var pressedColor: Meui.Theme.darkMode ? Qt.lighter(Meui.Theme.secondBackgroundColor, 1.8)
                                                 : Qt.darker(Meui.Theme.secondBackgroundColor, 1.5)

    property var highlightHoverColor: Meui.Theme.darkMode ? Qt.lighter(Meui.Theme.highlightColor, 1.2)
                                                          : Qt.darker(Meui.Theme.highlightColor, 1.1)
    property var highlightPressedColor: Meui.Theme.darkMode ? Qt.lighter(Meui.Theme.highlightColor, 1.1)
                                                            : Qt.darker(Meui.Theme.highlightColor, 1.2)

    MouseArea {
        id: _mouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton
        onClicked: control.clicked()

        onPressedChanged: {
            control.scale = pressed ? 0.95 : 1.0
        }
    }

    Behavior on scale {
        NumberAnimation {
            duration: 100
        }
    }

    Meui.RoundedRect {
        anchors.fill: parent
        radius: Meui.Theme.bigRadius
        backgroundOpacity: control.checked ? 0.9 : 0.3
        animationEnabled: false

        color: {
            if (control.checked) {
                if (_mouseArea.pressed)
                    return highlightPressedColor
                else if (_mouseArea.containsMouse)
                    return highlightHoverColor
                else
                    return Meui.Theme.highlightColor
            } else {
                if (_mouseArea.pressed)
                    return pressedColor
                else if (_mouseArea.containsMouse)
                    return hoverColor
                else
                    return Meui.Theme.secondBackgroundColor
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: Meui.Units.smallSpacing
        anchors.rightMargin: Meui.Units.smallSpacing

        Image {
            id: _image
            Layout.preferredWidth: control.height * 0.3
            Layout.preferredHeight: control.height * 0.3
            sourceSize: Qt.size(width, height)
            asynchronous: true
            Layout.alignment: Qt.AlignCenter
            Layout.topMargin: Meui.Units.largeSpacing

//            ColorOverlay {
//                anchors.fill: _image
//                source: _image
//                color: control.checked ? Meui.Theme.highlightedTextColor : Meui.Theme.disabledTextColor
//            }
        }

        Item {
            Layout.fillHeight: true
        }

        Label {
            id: _titleLabel
            color: control.checked ? Meui.Theme.highlightedTextColor : Meui.Theme.textColor
            Layout.preferredHeight: control.height * 0.15
            Layout.alignment: Qt.AlignHCenter
        }

        Item {
            Layout.fillHeight: true
        }

        Label {
            id: _label
            color: control.checked ? Meui.Theme.highlightedTextColor : Meui.Theme.textColor
            elide: Label.ElideRight
            Layout.preferredHeight: control.height * 0.1
            Layout.alignment: Qt.AlignHCenter
            // Layout.fillWidth: true
            Layout.bottomMargin: Meui.Units.largeSpacing
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
