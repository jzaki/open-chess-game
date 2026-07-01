#pragma once

#include <QString>
#include <QObject>
#include <QTimer>
#include <memory>

class NetworkManager : public QObject {
    Q_OBJECT

public:
    NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();

    void startBroadcasting(const QString& fen);
    void stopBroadcasting();
    void setGameMode(const QString& mode);

    QString getReceivedFen() const;
    bool hasNewState() const;
    void clearNewStateFlag();

signals:
    void stateReceived(QString fen);
    void modeChanged(QString mode);

private slots:
    void onHeartbeatTimer();
    void onTimeoutTimer();

private:
    std::unique_ptr<QTimer> m_heartbeatTimer;
    std::unique_ptr<QTimer> m_timeoutTimer;

    QString m_currentFen;
    QString m_receivedFen;
    QString m_gameMode;
    bool m_hasNewState;
    bool m_isNetworkMode;

    static const int HEARTBEAT_INTERVAL = 3000;
    static const int TIMEOUT_INTERVAL = 5000;

    void broadcastGameState();
    bool validateReceivedFen(const QString& fen);
};
