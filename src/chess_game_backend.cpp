#include "chess_game_backend.h"

ChessGameBackend::ChessGameBackend()
    : m_engine(std::make_unique<ChessEngine>()),
      m_ai(std::make_unique<AiOpponent>()),
      m_network(std::make_unique<NetworkManager>()),
      m_gameMode("solo"),
      m_difficulty(5)
{
    connect(m_network.get(), &NetworkManager::stateReceived, this, [this](const QString& fen) {
        processNetworkState(fen);
    });

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
        } else if (m_gameMode == "network") {
            m_network->startBroadcasting(m_engine->getFen());
        }

        return true;
    }
    return false;
}

void ChessGameBackend::resetGame()
{
    m_engine->reset();
    updateGameState();

    if (m_gameMode == "network") {
        m_network->startBroadcasting(m_engine->getFen());
    }
}

void ChessGameBackend::setGameMode(QString mode)
{
    if (mode == "solo" || mode == "network") {
        m_gameMode = mode;

        if (mode == "network") {
            m_network->setGameMode("network");
            m_network->startBroadcasting(m_engine->getFen());
        } else {
            m_network->stopBroadcasting();
        }
    }
}

void ChessGameBackend::setDifficulty(int level)
{
    m_difficulty = level;
    if (m_ai) {
        m_ai->setDifficulty(level);
    }
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

void ChessGameBackend::processNetworkState(const QString& fen)
{
    if (fen.isEmpty()) {
        m_engine->reset();
        updateGameState();
        return;
    }

    if (m_network->hasNewState()) {
        QString receivedFen = m_network->getReceivedFen();
        if (!receivedFen.isEmpty() && receivedFen != m_engine->getFen()) {
            m_engine->setPosition(receivedFen);
            updateGameState();
        }
        m_network->clearNewStateFlag();
    }
}
