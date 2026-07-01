#include "chess_game_backend.h"

ChessGameBackend::ChessGameBackend()
    : m_engine(std::make_unique<ChessEngine>()),
      m_ai(std::make_unique<AiOpponent>()),
      m_gameMode("solo"),
      m_difficulty(5)
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

        if (m_gameMode == "solo") {
            playAiMove();
        }

        return true;
    }
    return false;
}

void ChessGameBackend::resetGame()
{
    m_engine->reset();
    updateGameState();
}

void ChessGameBackend::setGameMode(QString mode)
{
    if (mode == "solo" || mode == "network") {
        m_gameMode = mode;
        setGameMode(mode);
    }
}

void ChessGameBackend::setDifficulty(int level)
{
    m_difficulty = level;
    if (m_ai) {
        m_ai->setDifficulty(level);
    }
    setDifficulty(level);
}

void ChessGameBackend::updateGameState()
{
    setFen(m_engine->getFen());
    setStatus(m_engine->getGameStatus());
}

void ChessGameBackend::playAiMove()
{
    QString fen = m_engine->getFen();
    bool isWhiteTurn = fen.contains(" w ");

    if (isWhiteTurn) {
        return;
    }

    QString bestMove = m_ai->getBestMove(fen);
    if (bestMove.length() >= 4) {
        QString from = bestMove.mid(0, 2);
        QString to = bestMove.mid(2, 2);

        if (m_engine->makeMove(from, to)) {
            updateGameState();
        }
    }
}
