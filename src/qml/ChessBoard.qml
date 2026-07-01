import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property var backend
    property string fen: ""
    property string selectedSquare: ""
    property var legalMoves: []

    width: 320
    height: 320

    function getPieceAt(square) {
        if (!root.fen) return "";

        const col = square.charCodeAt(0) - 97;
        const row = 8 - parseInt(square[1]);

        const fenParts = root.fen.split(' ');
        const position = fenParts[0];
        const rows = position.split('/');

        if (row < 0 || row > 7 || col < 0 || col > 7) return "";

        let currentCol = 0;
        for (let c of rows[row]) {
            if (!isNaN(c)) {
                currentCol += parseInt(c);
            } else {
                if (currentCol === col) return getPieceUnicode(c);
                currentCol++;
            }
        }
        return "";
    }

    function getPieceUnicode(piece) {
        const pieces = {
            'K': '♔', 'Q': '♕', 'R': '♖', 'B': '♗', 'N': '♘', 'P': '♙',
            'k': '♚', 'q': '♛', 'r': '♜', 'b': '♝', 'n': '♞', 'p': '♟'
        };
        return pieces[piece] || "";
    }

    Grid {
        id: boardGrid
        anchors.fill: parent
        columns: 8
        rows: 8
        spacing: 0

        Repeater {
            model: 64
            delegate: Rectangle {
                width: root.width / 8
                height: root.height / 8

                readonly property int row: Math.floor(index / 8)
                readonly property int col: index % 8
                readonly property string square: String.fromCharCode(97 + col) + (8 - row)
                readonly property bool isLight: (row + col) % 2 === 0

                color: isLight ? "#f0d9b5" : "#b58863"

                border.color: root.selectedSquare === square ? "#baca44" : "transparent"
                border.width: 2

                Text {
                    anchors.centerIn: parent
                    font.pixelSize: 36
                    font.family: "DejaVu Sans"
                    text: root.getPieceAt(square)
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (root.selectedSquare === "") {
                            root.selectedSquare = square;
                        } else if (root.selectedSquare === square) {
                            root.selectedSquare = "";
                        } else {
                            if (root.backend) {
                                root.backend.makeMove(root.selectedSquare, square);
                            }
                            root.selectedSquare = "";
                        }
                    }
                }
            }
        }
    }
}
