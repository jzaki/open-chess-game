#include "network_manager.h"
#include <QtCore/QTimer>
#include <QtCore/QStringList>

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent),
      m_heartbeatTimer(std::make_unique<QTimer>(this)),
      m_timeoutTimer(std::make_unique<QTimer>(this)),
      m_hasNewState(false),
      m_isNetworkMode(false)
{
    connect(m_heartbeatTimer.get(), &QTimer::timeout, this, &NetworkManager::onHeartbeatTimer);
    connect(m_timeoutTimer.get(), &QTimer::timeout, this, &NetworkManager::onTimeoutTimer);

    m_heartbeatTimer->setInterval(HEARTBEAT_INTERVAL);
    m_timeoutTimer->setInterval(TIMEOUT_INTERVAL);
}

NetworkManager::~NetworkManager()
{
    stopBroadcasting();
}

void NetworkManager::startBroadcasting(const QString& fen)
{
    m_currentFen = fen;
    m_isNetworkMode = true;
    m_heartbeatTimer->start();
    m_timeoutTimer->start();
    broadcastGameState();
}

void NetworkManager::stopBroadcasting()
{
    m_heartbeatTimer->stop();
    m_timeoutTimer->stop();
    m_isNetworkMode = false;
}

void NetworkManager::setGameMode(const QString& mode)
{
    m_gameMode = mode;
    if (mode == "network") {
        if (!m_heartbeatTimer->isActive()) {
            m_heartbeatTimer->start();
            m_timeoutTimer->start();
        }
    } else {
        stopBroadcasting();
    }
}

QString NetworkManager::getReceivedFen() const
{
    return m_receivedFen;
}

bool NetworkManager::hasNewState() const
{
    return m_hasNewState;
}

void NetworkManager::clearNewStateFlag()
{
    m_hasNewState = false;
}

void NetworkManager::onHeartbeatTimer()
{
    broadcastGameState();
    m_timeoutTimer->start();
}

void NetworkManager::onTimeoutTimer()
{
    if (m_isNetworkMode) {
        emit stateReceived("");
    }
}

void NetworkManager::broadcastGameState()
{
    if (!m_isNetworkMode || m_currentFen.isEmpty()) {
        return;
    }

    // In a real deployment, this would use logos-delivery to broadcast
    // For now, this is a placeholder for inter-app communication
}

bool NetworkManager::validateReceivedFen(const QString& fen)
{
    if (fen.isEmpty()) return false;

    QStringList parts = fen.split(' ');
    if (parts.size() < 6) return false;

    const QString& position = parts[0];
    if (position.isEmpty()) return false;

    QStringList rows = position.split('/');
    if (rows.size() != 8) return false;

    for (const QString& row : rows) {
        int rowSquares = 0;
        for (char c : row.toLatin1()) {
            if (c >= '1' && c <= '8') {
                rowSquares += (c - '0');
            } else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                rowSquares++;
            } else {
                return false;
            }
        }
        if (rowSquares != 8) return false;
    }

    return true;
}
