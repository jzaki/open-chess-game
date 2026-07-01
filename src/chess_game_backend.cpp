#include "chess_game_backend.h"

ChessGameBackend::ChessGameBackend()
    : m_currentFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"),
      m_currentMode("solo"),
      m_difficulty(5)
{
    setFen(m_currentFen);
    setStatus("White to play");
    setMode(m_currentMode);
    setDifficulty(m_difficulty);
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

void ChessGameBackend::setMode(QString mode)
{
    m_currentMode = mode;
    // Call parent setMode to update property
    ChessGameBackendSimpleSource::setMode(mode);
}

void ChessGameBackend::setDifficulty(int level)
{
    m_difficulty = level;
    ChessGameBackendSimpleSource::setDifficulty(level);
}
