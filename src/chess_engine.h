#pragma once

#include <QString>
#include <vector>
#include <array>

struct Move {
    QString from;
    QString to;
    bool isValid = false;
};

class ChessEngine {
public:
    ChessEngine();

    void setPosition(const QString& fen);
    QString getFen() const;

    bool isValidMove(const QString& fromSquare, const QString& toSquare) const;
    bool makeMove(const QString& fromSquare, const QString& toSquare);
    std::vector<Move> getLegalMoves() const;

    QString getGameStatus() const;
    bool isCheckmate() const;
    bool isStalemate() const;
    bool isCheck() const;

    void reset();

private:
    QString m_fen;
    std::array<std::array<char, 8>, 8> m_board;

    void fenToBoard();
    void boardToFen();
    bool isWhiteTurn() const;
    bool isPieceColor(char piece, bool isWhite) const;
    bool canMovePiece(const QString& from, const QString& to) const;
};
