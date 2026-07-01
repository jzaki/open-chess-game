#pragma once

#include "rep_chess_game_backend_source.h"
#include "logos_ui_plugin_context.h"
#include "chess_engine.h"
#include "ai_opponent.h"
#include "network_manager.h"
#include <memory>

class ChessGameBackend : public ChessGameBackendSimpleSource,
                         public LogosUiPluginContext
{
public:
    ChessGameBackend();

    bool makeMove(QString fromSquare, QString toSquare) override;
    void resetGame() override;
    void setGameMode(QString mode);
    void setDifficulty(int level);

private:
    std::unique_ptr<ChessEngine> m_engine;
    std::unique_ptr<AiOpponent> m_ai;
    std::unique_ptr<NetworkManager> m_network;
    QString m_gameMode;
    int m_difficulty;

    void updateGameState();
    void playAiMove();
    void processNetworkState(const QString& fen);
};
