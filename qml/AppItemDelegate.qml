import QtQuick 2.12
import QtQuick.Controls 2.5

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

    DockItem {
        anchors.fill: parent
        iconName: model.iconName
        isActive: model.isActive
        popupText: model.visibleName

        onClicked: root.toggleMinimize(model.appId)
    }
}
