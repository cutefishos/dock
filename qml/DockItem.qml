import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0
import Cutefish.Dock 1.0
import MeuiKit 1.0 as Meui

Item {
    id: control

    property bool isLeft: Settings.direction === DockSettings.Left
    property bool isRight: Settings.direction === DockSettings.Right
    property bool isBottom: Settings.direction === DockSettings.Bottom

    property var iconSize: root.isHorizontal ? control.height * iconSizeRatio
                                             : control.width * iconSizeRatio

    property bool draggable: false
    property int dragItemIndex

    property alias icon: icon
    property alias mouseArea: iconArea
    property alias dropArea: iconDropArea

    property bool enableActivateDot: true
    property bool isActive: false

    property var popupText

    property double iconSizeRatio: 0.8
    property var iconName

    property bool dragStarted: false

    signal positionChanged()
    signal released()
    signal pressed(var mouse)
    signal pressAndHold(var mouse)
    signal clicked(var mouse)
    signal rightClicked(var mouse)
    signal doubleClicked(var mouse)

    Drag.active: mouseArea.drag.active && control.draggable
    Drag.dragType: Drag.Automatic
    Drag.supportedActions: Qt.MoveAction
    Drag.hotSpot.x: icon.width / 2
    Drag.hotSpot.y: icon.height / 2

    Drag.onDragStarted:  {
        dragStarted = true
    }

    Drag.onDragFinished: {
        dragStarted = false
    }

    Meui.IconItem {
        id: icon
        anchors.centerIn: parent
        width: control.iconSize
        height: control.iconSize
        source: iconName

        visible: !dragStarted

        ColorOverlay {
            id: iconColorize
            anchors.fill: icon
            source: icon
            color: "#000000"
            opacity: iconArea.pressed && !mouseArea.drag.active ? 0.5 : 0
        }
    }

    DropArea {
        id: iconDropArea
        anchors.fill: icon
        enabled: draggable
    }

    MouseArea {
        id: iconArea
        anchors.fill: icon
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        drag.axis: Drag.XAndYAxis

        onClicked: {
            if (mouse.button === Qt.RightButton)
                control.rightClicked(mouse)
            else
                control.clicked(mouse)
        }

        onPressed: {
            control.pressed(mouse)
            popupTips.hide()
        }

        onPositionChanged: {
            if (pressed) {
                if (mouse.source !== Qt.MouseEventSynthesizedByQt) {
                    drag.target = icon
                    icon.grabToImage(function(result) {
                        control.Drag.imageSource = result.url
                    })
                } else {
                    drag.target = null
                }
            }

            control.positionChanged()
        }

        onPressAndHold : control.pressAndHold(mouse)
        onReleased: {
            drag.target = null
            control.released()
        }

        onContainsMouseChanged: {
            if (containsMouse && control.popupText !== "") {
                popupTips.popupText = control.popupText

                if (Settings.direction === DockSettings.Left)
                    popupTips.position = Qt.point(root.width + Meui.Units.largeSpacing,
                                                  control.mapToGlobal(0, 0).y + (control.height / 2 - popupTips.height / 2))
                else if (Settings.direction === DockSettings.Right)
                    popupTips.position = Qt.point(control.mapToGlobal(0, 0).x - popupTips.width - Meui.Units.smallSpacing / 2,
                                                  control.mapToGlobal(0, 0).y + (control.height / 2 - popupTips.height / 2))
                else
                    popupTips.position = Qt.point(control.mapToGlobal(0, 0).x + (control.width / 2 - popupTips.width / 2),
                                                  control.mapToGlobal(0, 0).y - popupTips.height - Meui.Units.smallSpacing)

                popupTips.show()
            } else {
                popupTips.hide()
            }
        }
    }

    Rectangle {
        id: activeLine
        width: !isBottom ? parent.width * 0.06 : (isActive ? parent.height * 0.4 : parent.height * 0.06)
        height: !isBottom ? (isActive ? parent.height * 0.4 : parent.height * 0.06) : parent.height * 0.06
        color: Meui.Theme.textColor
        radius: !isBottom ? width / 2 : height / 2
        visible: enableActivateDot && !dragStarted
        opacity: isActive ? 1 : 0.6

        property var leftX: 3
        property var leftY: (parent.height - height) / 2

        property var bottomX: (parent.width - width) / 2
        property var bottomY: icon.y + icon.height + activeLine.height / 2 - 2

        property var rightX: icon.x + icon.width + activeLine.width / 2 - 2
        property var rightY: (parent.height - height) / 2

        x: isLeft ? leftX : isBottom ? bottomX : rightX
        y: isLeft ? leftY : isBottom ? bottomY : rightY

        Behavior on opacity {
            NumberAnimation {
                duration: 125
                easing.type: Easing.InOutCubic
            }
        }

        Behavior on width {
            NumberAnimation {
                duration: isBottom ? 125 : 0
                easing.type: Easing.InOutCubic
            }
        }

        Behavior on height {
            NumberAnimation {
                duration: !isBottom ? 125 : 0
                easing.type: Easing.InOutCubic
            }
        }
    }
}
