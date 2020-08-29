import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    id: dockItem
    width: root.height
    height: root.height

    property bool enableActivateDot: true
    property bool isActive: false
    property var activateDotColor: "#2E64E6"
    property var inactiveDotColor: "#000000"

    property double iconSizeRatio: 0.8
    property var iconSource

    Image {
        id: icon
        source: iconSource
        sourceSize.width: parent.height * iconSizeRatio
        sourceSize.height: parent.height * iconSizeRatio

        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
    }

    Rectangle {
        id: activeDot
        width: parent.height * 0.07
        height: width
        color: isActive ? activateDotColor : inactiveDotColor
        radius: height
        visible: enableActivateDot

        anchors {
            top: icon.bottom
            horizontalCenter: parent.horizontalCenter
        }
    }
}
