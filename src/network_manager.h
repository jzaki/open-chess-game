#pragma once

#include <QString>
#include <QObject>
#include <QTimer>
#include <memory>

class LogosUiPluginContext;

class NetworkManager : public QObject {
    Q_OBJECT

public:
    NetworkManager(LogosUiPluginContext* context, QObject *parent = nullptr);
    ~NetworkManager();

    void onContextReady();
    void startBroadcasting(const QString& fen);
    void stopBroadcasting();

    QString getReceivedFen() const;
    bool hasNewState() const;
    void clearNewStateFlag();

signals:
    void stateReceived(QString fen);

private slots:
    void bootstrap();
    void onHeartbeatTimer();
    void onMessageReceived(const QVariantList& data);

private:
    LogosUiPluginContext* m_context;
    std::unique_ptr<QTimer> m_heartbeatTimer;

    QString m_currentFen;
    QString m_receivedFen;
    bool m_hasNewState;
    bool m_isNetworkMode;
    bool m_nodeReady;

    static const int HEARTBEAT_INTERVAL = 3000;
    static const QString CHESS_TOPIC;

    void broadcastGameState();
    bool validateReceivedFen(const QString& fen);
};
