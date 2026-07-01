#pragma once

#include <QString>
#include <vector>
#include "chess_engine.h"

class AiOpponent {
public:
    AiOpponent();

    void setDifficulty(int level);
    int getDifficulty() const;

    QString getBestMove(const QString& fen);

private:
    int m_difficulty;
    int m_maxDepth;

    struct ScoredMove {
        Move move;
        int score = 0;
    };

    std::vector<ScoredMove> scoreAllMoves(const QString& fen, ChessEngine& engine);
    int evaluateMove(const Move& move, ChessEngine& engine);
    int evaluatePosition(const QString& fen);
    bool isCaptureMove(const Move& move, ChessEngine& engine);
    bool isCheckingMove(const Move& move, ChessEngine& engine);
};
