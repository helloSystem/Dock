import QtQuick 2.12
import QtQuick.Controls 2.5
import Qt.labs.platform 1.0

Item {
    id: appItem
    width: root.height
    height: root.height

    property bool enableActivateDot: true
    property bool isActive: model.isActive
    property var activateDotColor: "#2E64E6"
    property var inactiveDotColor: "#000000"

    property var iconName: model.iconName
    property double iconSizeRatio: 0.8
    property var iconSource

    signal onClicked

    Menu {
        id: contextMenu

        MenuItem {
            text: qsTr("Open")
            visible: model.windowCount === 0
            onTriggered: appModel.openNewInstance(model.appId)
        }

        MenuItem {
            text: model.visibleName
            visible: model.windowCount > 0
            onTriggered: appModel.openNewInstance(model.appId)
        }

        MenuItem {
            text: model.isPined ? qsTr("Unpin") : qsTr("Pin")
            onTriggered: {}
        }

        MenuItem {
            text: qsTr("Close All")
            onTriggered: appModel.closeAllByAppId(model.appId)
        }
    }

    DockItem {
        anchors.fill: parent
        iconName: model.iconName
        isActive: model.isActive
        popupText: model.visibleName

        onClicked: appModel.clicked(model.appId)
        onRightClicked: contextMenu.open()
    }
}
