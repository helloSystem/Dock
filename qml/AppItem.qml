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

    onXChanged: updateGeometryTimer.start()
    onYChanged: updateGeometryTimer.start()

    function updateGeometry() {
        appModel.updateGeometries(model.appId, Qt.rect(dockItem.mapToGlobal(0, 0).x,
                                                            dockItem.mapToGlobal(0, 0).y,
                                                            dockItem.width, dockItem.height))
    }

    Timer {
        id: updateGeometryTimer
        interval: 800
        repeat: false

        onTriggered: {
            updateGeometry()
        }
    }

    Menu {
        id: contextMenu

        /*
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
        */

        MenuItem {
            // visible: model.visibleName !== {};
            // FIXME: Only show this menu item if model.exec is present and not empty; why can't we seem to get model.exec here?
            text: model.isPined ? qsTr("Remove ") + model.visibleName + qsTr(" from Dock") : qsTr("Keep ") + model.visibleName + qsTr(" in Dock")
            onTriggered: {
                model.isPined ? appModel.unPin(model.appId) : appModel.pin(model.appId)
            }
        }

        MenuItem {
            text: qsTr("Close ") + model.visibleName
            visible: model.windowCount !== 0
            onTriggered: appModel.closeAllByAppId(model.appId)
        }
    }

    DockItem {
        id: dockItem
        anchors.fill: parent
        iconName: model.iconName
        isActive: model.isActive
        popupText: model.visibleName
        enableActivateDot: model.windowCount !== 0

        onPositionChanged: updateGeometry()
        onPressed: updateGeometry()
        onClicked: appModel.clicked(model.appId)
        onRightClicked: contextMenu.open()
    }
}
