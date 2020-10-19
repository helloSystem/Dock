import QtQuick 2.4
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

Rectangle {
    id: root
    visible: false

    property int padding: 20

    width: metrics.boundingRect.width + padding
    height: metrics.boundingRect.height + padding

    radius: 4
    color: Settings.darkMode ? Qt.rgba(0, 0, 0, 0.3) : Qt.rgba(255, 255, 255, 0.6)

    border.color: Settings.darkMode ? Qt.rgba(255, 255, 255, 0.2) : Qt.rgba(0, 0, 0, 0.2)
    border.width: 1

    Label {
        id: label
        color: Settings.darkMode ? Qt.rgba(255, 255, 255, 0.7) : Qt.rgba(0, 0, 0, 0.7)
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
