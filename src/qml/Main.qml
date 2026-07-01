import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    readonly property var backend: logos.module("chess_game")
    property bool ready: false

    readonly property string fen: backend ? backend.fen : ""
    readonly property string status: backend ? backend.status : ""

    Connections {
        target: logos
        function onViewModuleReadyChanged(moduleName, isReady) {
            if (moduleName === "chess_game")
                root.ready = isReady && root.backend !== null;
        }
    }
    Component.onCompleted: {
        root.ready = root.backend !== null && logos.isViewModuleReady("chess_game");
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Text {
            text: "Chess Game"
            font.pixelSize: 20
            color: "#ffffff"
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: root.ready ? "Connected" : "Connecting to backend..."
            color: root.ready ? "#56d364" : "#f0883e"
            font.pixelSize: 12
        }

        Button {
            text: "New Game"
            enabled: root.ready
            onClicked: backend.resetGame()
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Status: " + root.status
            color: "#ffffff"
            font.pixelSize: 15
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "FEN: " + root.fen
            color: "#8b949e"
            font.pixelSize: 11
            wrapMode: Text.Wrap
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
