import QtQuick 2.12
import QtQuick.Controls 2.5

Rectangle {
    visible: true
    id: root

//    color: "red"

    DockItem {
        id: launcherItem
        anchors.left: parent.left
        anchors.top: parent.top

        iconSizeRatio: 0.7
        enableActivateDot: false
        iconSource: "file:///usr/share/icons/Lucia/apps/scalable/launcher.svg"
    }

    Rectangle {
        id: appList
        anchors.left: launcherItem.right
        anchors.top: parent.top
        width: parent.width - launcherItem.width * 2
        height: parent.height
        // color: "transparent"

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
