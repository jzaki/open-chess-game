import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property string status: "White to play"
    property string fen: ""

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Text {
            text: "Game Status"
            font.bold: true
            font.pixelSize: 14
            color: "#ffffff"
        }

        Text {
            text: root.status
            font.pixelSize: 16
            color: "#56d364"
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        Rectangle {
            height: 1
            color: "#444444"
            Layout.fillWidth: true
        }

        Text {
            text: "Position (FEN)"
            font.bold: true
            font.pixelSize: 12
            color: "#ffffff"
        }

        Text {
            text: root.fen
            font.pixelSize: 10
            color: "#8b949e"
            wrapMode: Text.Wrap
            Layout.fillWidth: true
            maximumLineCount: 3
            elide: Text.ElideRight
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
