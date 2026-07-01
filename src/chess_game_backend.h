#pragma once

#include "rep_chess_game_backend_source.h"
#include "logos_ui_plugin_context.h"
#include "chess_engine.h"
#include <memory>

class ChessGameBackend : public ChessGameBackendSimpleSource,
                         public LogosUiPluginContext
{
public:
    ChessGameBackend();

    bool makeMove(QString fromSquare, QString toSquare) override;
    void resetGame() override;

private:
    std::unique_ptr<ChessEngine> m_engine;

    void updateGameState();
};
