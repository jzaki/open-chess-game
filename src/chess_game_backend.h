#pragma once

#include "rep_chess_game_backend_source.h"
#include "logos_ui_plugin_context.h"

class ChessGameBackend : public ChessGameBackendSimpleSource,
                         public LogosUiPluginContext
{
public:
    ChessGameBackend();

    bool makeMove(QString fromSquare, QString toSquare) override;
    void resetGame() override;
    void setMode(QString mode) override;
    void setDifficulty(int level) override;

private:
    QString m_currentFen;
    QString m_currentMode;
    int m_difficulty;
};
