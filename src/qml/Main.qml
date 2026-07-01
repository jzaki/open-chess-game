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
        anchors.margins: 16
        spacing: 12

        Text {
            text: "Chess Game"
            font.pixelSize: 18
            font.bold: true
            color: "#ffffff"
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: root.ready ? "Connected" : "Connecting..."
            color: root.ready ? "#56d364" : "#f0883e"
            font.pixelSize: 11
            Layout.alignment: Qt.AlignHCenter
        }

        RowLayout {
            spacing: 16
            Layout.fillHeight: true

            ChessBoard {
                id: board
                backend: root.backend
                fen: root.fen
                Layout.preferredWidth: 320
                Layout.preferredHeight: 320
            }

            ColumnLayout {
                spacing: 8
                Layout.fillHeight: true
                Layout.fillWidth: true

                GameStatus {
                    status: root.status
                    fen: root.fen
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }

                Button {
                    text: "New Game"
                    enabled: root.ready
                    onClicked: backend.resetGame()
                    Layout.fillWidth: true
                }

                Item {
                    Layout.fillHeight: true
                }
            }
        }
    }
}
