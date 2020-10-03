import QtQuick 2.4
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

Rectangle {
    id: root
    visible: false

    property int padding: 20

    width: metrics.boundingRect.width + padding
    height: metrics.boundingRect.height + padding

    radius: 3
    opacity: 0.6
    color: "white"

    border.color: Qt.rgba(0, 0, 0, 0.2)
    border.width: 1

    Label {
        id: label
        color: "black"
        text: "tips"
        anchors.centerIn: parent
    }

    TextMetrics {
        id: metrics
        font: label.font
        text: label.text
    }

    function setText(text) {
        label.text = text
    }

    function setVisible(visible) {
        root.visible = visible
    }
}
