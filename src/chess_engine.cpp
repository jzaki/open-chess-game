#include "chess_engine.h"
#include <cctype>
#include <cstdlib>
#include <QtCore/QStringList>

ChessEngine::ChessEngine()
    : m_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
{
    fenToBoard();
}

void ChessEngine::setPosition(const QString& fen)
{
    m_fen = fen;
    fenToBoard();
}

QString ChessEngine::getFen() const
{
    return m_fen;
}

void ChessEngine::fenToBoard()
{
    int row = 0, col = 0;
    for (char c : m_fen.toLatin1()) {
        if (c == ' ') break;
        if (c == '/') {
            row++;
            col = 0;
        } else if (std::isdigit(c)) {
            col += (c - '0');
        } else {
            m_board[row][col] = c;
            col++;
        }
    }
}

void ChessEngine::boardToFen()
{
    QString fen;
    for (int row = 0; row < 8; row++) {
        int emptyCount = 0;
        for (int col = 0; col < 8; col++) {
            char piece = m_board[row][col];
            if (piece == 0) {
                emptyCount++;
            } else {
                if (emptyCount > 0) {
                    fen += QString::number(emptyCount);
                    emptyCount = 0;
                }
                fen += piece;
            }
        }
        if (emptyCount > 0) {
            fen += QString::number(emptyCount);
        }
        if (row < 7) fen += '/';
    }

    m_fen = fen + " w KQkq - 0 1";
}

bool ChessEngine::isWhiteTurn() const
{
    QStringList parts = m_fen.split(' ');
    return parts.size() > 1 && parts[1] == "w";
}

bool ChessEngine::isPieceColor(char piece, bool isWhite) const
{
    if (!piece) return false;
    return isWhite ? std::isupper(piece) : std::islower(piece);
}

bool ChessEngine::isValidMove(const QString& fromSquare, const QString& toSquare) const
{
    if (fromSquare.length() != 2 || toSquare.length() != 2) return false;

    int fromCol = fromSquare[0].toLatin1() - 'a';
    int fromRow = 8 - (fromSquare[1].toLatin1() - '0');
    int toCol = toSquare[0].toLatin1() - 'a';
    int toRow = 8 - (toSquare[1].toLatin1() - '0');

    if (fromCol < 0 || fromCol > 7 || fromRow < 0 || fromRow > 7) return false;
    if (toCol < 0 || toCol > 7 || toRow < 0 || toRow > 7) return false;

    char piece = m_board[fromRow][fromCol];
    if (!piece) return false;

    bool isWhite = std::isupper(piece);
    if (isWhite != isWhiteTurn()) return false;

    char targetPiece = m_board[toRow][toCol];
    if (targetPiece && isPieceColor(targetPiece, isWhite)) return false;

    return canMovePiece(fromSquare, toSquare);
}

bool ChessEngine::canMovePiece(const QString& from, const QString& to) const
{
    int fromCol = from[0].toLatin1() - 'a';
    int fromRow = 8 - (from[1].toLatin1() - '0');
    int toCol = to[0].toLatin1() - 'a';
    int toRow = 8 - (to[1].toLatin1() - '0');

    char piece = std::tolower(m_board[fromRow][fromCol]);

    switch (piece) {
    case 'p': {
        int direction = std::isupper(m_board[fromRow][fromCol]) ? -1 : 1;
        if (toCol == fromCol && m_board[toRow][toCol] == 0) {
            if (toRow == fromRow + direction) return true;
            if ((fromRow == 6 || fromRow == 1) && toRow == fromRow + 2 * direction &&
                m_board[fromRow + direction][fromCol] == 0) return true;
        }
        if (std::abs(toCol - fromCol) == 1 && toRow == fromRow + direction &&
            m_board[toRow][toCol] != 0) return true;
        return false;
    }
    case 'n':
        return (std::abs(toRow - fromRow) == 2 && std::abs(toCol - fromCol) == 1) ||
               (std::abs(toRow - fromRow) == 1 && std::abs(toCol - fromCol) == 2);

    case 'b':
        return std::abs(toRow - fromRow) == std::abs(toCol - fromCol);

    case 'r':
        return toRow == fromRow || toCol == fromCol;

    case 'q':
        return toRow == fromRow || toCol == fromCol ||
               std::abs(toRow - fromRow) == std::abs(toCol - fromCol);

    case 'k':
        return std::abs(toRow - fromRow) <= 1 && std::abs(toCol - fromCol) <= 1;

    default:
        return false;
    }
}

bool ChessEngine::makeMove(const QString& fromSquare, const QString& toSquare)
{
    if (!isValidMove(fromSquare, toSquare)) return false;

    int fromCol = fromSquare[0].toLatin1() - 'a';
    int fromRow = 8 - (fromSquare[1].toLatin1() - '0');
    int toCol = toSquare[0].toLatin1() - 'a';
    int toRow = 8 - (toSquare[1].toLatin1() - '0');

    m_board[toRow][toCol] = m_board[fromRow][fromCol];
    m_board[fromRow][fromCol] = 0;

    boardToFen();
    return true;
}

std::vector<Move> ChessEngine::getLegalMoves() const
{
    std::vector<Move> moves;
    for (int fromRow = 0; fromRow < 8; fromRow++) {
        for (int fromCol = 0; fromCol < 8; fromCol++) {
            if (!m_board[fromRow][fromCol]) continue;

            char from[3] = {(char)('a' + fromCol), (char)('8' - fromRow), 0};
            for (int toRow = 0; toRow < 8; toRow++) {
                for (int toCol = 0; toCol < 8; toCol++) {
                    char to[3] = {(char)('a' + toCol), (char)('8' - toRow), 0};
                    if (isValidMove(from, to)) {
                        moves.push_back({QString(from), QString(to), true});
                    }
                }
            }
        }
    }
    return moves;
}

QString ChessEngine::getGameStatus() const
{
    bool whiteTurn = isWhiteTurn();
    QString status = whiteTurn ? "White to play" : "Black to play";

    if (isCheckmate()) {
        status = whiteTurn ? "Checkmate: Black wins" : "Checkmate: White wins";
    } else if (isStalemate()) {
        status = "Stalemate: Draw";
    } else if (isCheck()) {
        status += " (Check)";
    }

    return status;
}

bool ChessEngine::isCheckmate() const
{
    return isCheck() && getLegalMoves().empty();
}

bool ChessEngine::isStalemate() const
{
    return !isCheck() && getLegalMoves().empty();
}

bool ChessEngine::isCheck() const
{
    return false;
}

void ChessEngine::reset()
{
    m_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    fenToBoard();
}
