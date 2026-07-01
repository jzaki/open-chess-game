#pragma once

#include "rep_chess_game_backend_source.h"
#include "logos_ui_plugin_context.h"
#include "chess_engine.h"
#include "ai_opponent.h"
#include <memory>

class ChessGameBackend : public ChessGameBackendSimpleSource,
                         public LogosUiPluginContext
{
public:
    ChessGameBackend();

    bool makeMove(QString fromSquare, QString toSquare) override;
    void resetGame() override;
    void setGameMode(QString mode) override;
    void setDifficulty(int level) override;

private:
    std::unique_ptr<ChessEngine> m_engine;
    std::unique_ptr<AiOpponent> m_ai;
    QString m_gameMode;
    int m_difficulty;

    void updateGameState();
    void playAiMove();
};
