import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    readonly property var backend: logos.module("chess_game")
    property bool ready: false

    readonly property string fen: backend ? backend.fen : ""
    readonly property string status: backend ? backend.status : ""
    readonly property string mode: backend ? backend.mode : "solo"

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

        RowLayout {
            spacing: 12
            Layout.fillWidth: true

            Button {
                text: "New Game"
                enabled: root.ready
                onClicked: backend.resetGame()
            }

            ComboBox {
                model: ["solo", "network"]
                enabled: root.ready
                currentIndex: root.mode === "network" ? 1 : 0
                onCurrentValueChanged: backend.setMode(currentValue)
            }

            Button {
                text: "Solo"
                enabled: root.mode === "solo"
            }
        }

        Text {
            text: "Game Status: " + root.status
            color: "#ffffff"
            font.pixelSize: 15
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
