import QtQuick 2.12
import QtQuick.Controls 2.12
// import Qt.labs.platform 1.0
import Cutefish.Dock 1.0

DockItem {
    id: appItem

    property var windowCount: model.windowCount

    iconName: model.iconName ? model.iconName : "application-x-desktop"
    isActive: model.isActive
    popupText: model.visibleName
    enableActivateDot: windowCount !== 0
    draggable: model.appId === "cutefish-launcher" ? false : true
    dragItemIndex: index

    onWindowCountChanged: {
        if (windowCount > 0)
            updateGeometry()
    }

    onPositionChanged: updateGeometry()
    onPressed: updateGeometry()
    onClicked: appModel.clicked(model.appId)
    onRightClicked: if (model.appId !== "cutefish-launcher") contextMenu.show()

    dropArea.onEntered: {
        if (drag.source)
            appModel.move(drag.source.dragItemIndex, appItem.dragItemIndex)
        else
            appModel.raiseWindow(model.appId)
    }

    dropArea.onDropped: {
        appModel.save()
        updateGeometry()
    }

    DockMenu {
        id: contextMenu

        MenuItem {
            text: qsTr("Open")
            visible: windowCount === 0
            onTriggered: appModel.openNewInstance(model.appId)
        }

        MenuItem {
            text: model.visibleName
            visible: windowCount > 0
            onTriggered: appModel.openNewInstance(model.appId)
        }

        MenuItem {
            text: model.isPinned ? qsTr("Unpin") : qsTr("Pin")
            onTriggered: {
                model.isPinned ? appModel.unPin(model.appId) : appModel.pin(model.appId)
            }
        }

        MenuItem {
            text: qsTr("Close All")
            visible: windowCount !== 0
            onTriggered: appModel.closeAllByAppId(model.appId)
        }
    }


    function updateGeometry() {
        appModel.updateGeometries(model.appId, Qt.rect(appItem.mapToGlobal(0, 0).x,
                                                       appItem.mapToGlobal(0, 0).y,
                                                       appItem.width, appItem.height))
    }
}
