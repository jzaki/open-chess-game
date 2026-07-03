#include "chess_game_backend.h"
#include <QtCore/QDebug>

ChessGameBackend::ChessGameBackend()
    : m_gameMode("solo"),
      m_difficulty(5)
{
    m_engine = std::make_unique<ChessEngine>();
    m_ai = std::make_unique<AiOpponent>();
    m_network = std::make_unique<NetworkManager>(this);

    if (m_network) {
        connect(m_network.get(), &NetworkManager::stateReceived, this, [this](const QString& fen) {
            processNetworkState(fen);
        });
    }

    updateGameState();
}

void ChessGameBackend::setGameMode(QString mode)
{
    qDebug() << "setGameMode called with mode=" << mode << ", m_gameMode was=" << m_gameMode;
    if (mode == "solo" || mode == "network") {
        m_gameMode = mode;
        qDebug() << "Mode set to" << m_gameMode;

        if (mode == "network") {
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

void ChessGameBackend::onContextReady()
{
    if (m_network) {
        m_network->onContextReady();
    }
}

bool ChessGameBackend::makeMove(QString fromSquare, QString toSquare)
{
    if (!m_engine->isValidMove(fromSquare, toSquare)) {
        return false;
    }

    if (m_engine->makeMove(fromSquare, toSquare)) {
        updateGameState();

        qDebug() << "Move made, m_gameMode =" << m_gameMode;
        if (m_gameMode == "solo") {
            qDebug() << "Playing AI move (solo mode)";
            playAiMove();
        } else if (m_gameMode == "network") {
            qDebug() << "Broadcasting game state (network mode)";
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
