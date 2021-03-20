import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12

Window {
    id: control
    visible: false

    width: _mainLayout.implicitWidth
    height: _mainLayout.implicitHeight

    flags: Qt.Popup

    onActiveChanged: {
        if (!active)
            control.close()
    }

    default property alias content: _mainLayout.data

    function open() {
        control.visible = true
        control.x = 500
        control.y = 500
    }

    ColumnLayout {
        id: _mainLayout
        anchors.fill: parent
    }
}
