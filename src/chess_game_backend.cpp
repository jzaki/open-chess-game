#include "chess_game_backend.h"

ChessGameBackend::ChessGameBackend()
    : m_currentFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
{
    setFen(m_currentFen);
    setStatus("White to play");
}

bool ChessGameBackend::makeMove(QString fromSquare, QString toSquare)
{
    // TODO: Validate move, update FEN, check game status
    return false;
}

void ChessGameBackend::resetGame()
{
    m_currentFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    setFen(m_currentFen);
    setStatus("White to play");
}
