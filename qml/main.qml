import QtQuick 2.12
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0

Rectangle {
    visible: true
    id: root

    color: "transparent"

    Rectangle {
        id: background
        anchors.fill: parent
        radius: parent.height * 0.3
        opacity: 0.6
        color: "white"
    }

    DockItem {
        id: launcherItem
        anchors.left: parent.left
        anchors.top: parent.top

        iconSizeRatio: 0.7
        enableActivateDot: false
        iconSource: "file:///usr/share/icons/Lucia/apps/scalable/launcher.svg"
    }

    Item {
        id: appList
        anchors.left: launcherItem.right
        anchors.top: parent.top
        width: parent.width - launcherItem.width * 2
        height: parent.height

        ListView {
            id: pageView
            anchors.fill: parent
            orientation: Qt.Horizontal
            snapMode: ListView.SnapOneItem
            model: appModel
            clip: true

            delegate: AppItemDelegate { }
        }
    }

    DockItem {
        id: trashItem
        anchors.left: appList.right
        anchors.top: parent.top

        iconSizeRatio: 0.7
        enableActivateDot: false
        iconSource: "file:///home/reven/Panda/panda-dock/resources/user-trash-full.svg"
    }

    function toggleMinimize(wid) {
        appModel.toggleMinimize(wid)
    }
}
