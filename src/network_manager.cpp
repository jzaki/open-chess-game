#include "network_manager.h"
#include "logos_ui_plugin_context.h"
#include <QtCore/QTimer>
#include <QtCore/QStringList>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QDateTime>
#include <QtCore/QVariantList>

#include "logos_sdk.h"
#include "logos_types.h"

const QString NetworkManager::CHESS_TOPIC = "/chessapp/1/moves/proto";

NetworkManager::NetworkManager(LogosUiPluginContext* context, QObject *parent)
    : QObject(parent),
      m_context(context),
      m_heartbeatTimer(std::make_unique<QTimer>(this)),
      m_hasNewState(false),
      m_isNetworkMode(false),
      m_nodeReady(false)
{
    connect(m_heartbeatTimer.get(), &QTimer::timeout, this, &NetworkManager::onHeartbeatTimer);
    m_heartbeatTimer->setInterval(HEARTBEAT_INTERVAL);
}

NetworkManager::~NetworkManager()
{
    stopBroadcasting();
}

void NetworkManager::onContextReady()
{
    if (!m_context) return;
    QTimer::singleShot(0, this, &NetworkManager::bootstrap);
}

void NetworkManager::bootstrap()
{
    if (!m_context) return;

    m_context->modules().delivery_module.on(
        "messageReceived", [this](const QVariantList& data) {
            onMessageReceived(data);
        });

    const QJsonObject cfg{
        {"logLevel", "INFO"},
        {"mode", "Core"},
        {"preset", "logos.test"},
    };
    const QString cfgJson = QString::fromUtf8(QJsonDocument(cfg).toJson(QJsonDocument::Compact));

    LogosResult created = m_context->modules().delivery_module.createNode(cfgJson);
    if (created.success) {
        LogosResult started = m_context->modules().delivery_module.start();
        if (!started.success) {
            return;
        }
    }

    LogosResult subscribed = m_context->modules().delivery_module.subscribe(CHESS_TOPIC);
    if (!subscribed.success) {
        return;
    }

    m_nodeReady = true;
}

void NetworkManager::startBroadcasting(const QString& fen)
{
    m_currentFen = fen;
    m_isNetworkMode = true;

    if (!m_nodeReady) {
        onContextReady();
    }

    m_heartbeatTimer->start();
    broadcastGameState();
}

void NetworkManager::stopBroadcasting()
{
    m_heartbeatTimer->stop();
    m_isNetworkMode = false;
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

void NetworkManager::broadcastGameState()
{
    if (!m_isNetworkMode || m_currentFen.isEmpty() || !m_nodeReady || !m_context) {
        return;
    }

    QJsonObject payload;
    payload["fen"] = m_currentFen;

    QJsonDocument doc(payload);
    QByteArray jsonBytes = doc.toJson(QJsonDocument::Compact);

    m_context->modules().delivery_module.send(CHESS_TOPIC, jsonBytes);
}

void NetworkManager::onHeartbeatTimer()
{
    broadcastGameState();
}

void NetworkManager::onMessageReceived(const QVariantList& data)
{
    if (data.size() < 3) return;

    QByteArray payload = data.at(2).toByteArray();
    QString jsonText = QString::fromUtf8(payload);

    QJsonDocument doc = QJsonDocument::fromJson(jsonText.toUtf8());
    if (!doc.isObject()) return;

    QJsonObject obj = doc.object();
    QString receivedFen = obj.value("fen").toString();

    if (validateReceivedFen(receivedFen) && receivedFen != m_currentFen) {
        m_receivedFen = receivedFen;
        m_hasNewState = true;
        emit stateReceived(receivedFen);
    }
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
