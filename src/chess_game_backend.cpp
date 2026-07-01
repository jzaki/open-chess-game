#include "chess_game_backend.h"

ChessGameBackend::ChessGameBackend()
    : m_engine(std::make_unique<ChessEngine>())
{
    updateGameState();
}

bool ChessGameBackend::makeMove(QString fromSquare, QString toSquare)
{
    if (!m_engine->isValidMove(fromSquare, toSquare)) {
        return false;
    }

    if (m_engine->makeMove(fromSquare, toSquare)) {
        updateGameState();
        return true;
    }
    return false;
}

void ChessGameBackend::resetGame()
{
    m_engine->reset();
    updateGameState();
}

void ChessGameBackend::updateGameState()
{
    setFen(m_engine->getFen());
    setStatus(m_engine->getGameStatus());
}
