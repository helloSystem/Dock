import QtQuick 2.12
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0

Rectangle {
    visible: true
    id: root

    color: "transparent"

    property color backgroundColor: Settings.darkMode ? Qt.rgba(0, 0, 0, 0.05) : Qt.rgba(255, 255, 255, 0.5)
    property color foregroundColor: Settings.darkMode ? "white" : "black"
    property color borderColor: Settings.darkMode ? Qt.rgba(255, 255, 255, 0.1) : Qt.rgba(0, 0, 0, 0.05)
    property color activateDotColor: Settings.darkMode ? "#4d81ff" : "#2E64E6"
    property color inactiveDotColor: Settings.darkMode ? Qt.rgba(255, 255, 255, 0.6) : Qt.rgba(0, 0, 0, 0.9)

    y: 350
    NumberAnimation on y {
        to: 0
        duration: 5000
        easing {type: Easing.OutQuad}
    }

    Rectangle {
        id: background
        anchors.fill: parent
        radius: 1
        opacity: 0.2
        color: "white"
    }


    Item {
        id: appList
        anchors.left: parent.left
        anchors.top: parent.top
        width: parent.width - trashItem.width
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
        popupText: qsTr("Trash")

        iconSizeRatio: 0.75
        enableActivateDot: false
        iconName: "user-trash-empty"

        onClicked: {
            process.start("gio", ["open", "trash:///"])
        }
    }
}
