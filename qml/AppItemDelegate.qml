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

    Image {
        id: icon
        source: {
            return iconName ? iconName.indexOf("/") === 0 || iconName.indexOf("file://") === 0 || iconName.indexOf("qrc") === 0
                              ? iconName : "image://icontheme/" + iconName : iconName;
        }
        sourceSize.width: parent.height * iconSizeRatio
        sourceSize.height: parent.height * iconSizeRatio
        width: sourceSize.width
        height: sourceSize.height
        smooth: true

        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }

        states: ["mouseIn", "mouseOut"]
        state: "mouseOut"

        transitions: [
            Transition {
                from: "*"
                to: "mouseIn"

                NumberAnimation {
                    target: icon
                    properties: "scale"
                    from: 1
                    to: 1.1
                    duration: 150
                    easing.type: Easing.OutCubic
                }
            },
            Transition {
                from: "*"
                to: "mouseOut"

                NumberAnimation {
                    target: icon
                    properties: "scale"
                    from: 1.1
                    to: 1
                    duration: 150
                    easing.type: Easing.InCubic
                }
            }
        ]
    }

    MouseArea {
        id: iconArea
        anchors.fill: icon
        hoverEnabled: true

        onClicked: {
             root.toggleMinimize(model.appId)
        }

        onContainsMouseChanged: {
            icon.state = containsMouse ? "mouseIn" : "mouseOut"
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
