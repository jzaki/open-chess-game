#include "ai_opponent.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <QtCore/QStringList>

AiOpponent::AiOpponent() : m_difficulty(5), m_maxDepth(2)
{
    std::srand(std::time(nullptr));
}

void AiOpponent::setDifficulty(int level)
{
    m_difficulty = std::max(1, std::min(10, level));
    m_maxDepth = 1 + (m_difficulty / 3);
}

int AiOpponent::getDifficulty() const
{
    return m_difficulty;
}

QString AiOpponent::getBestMove(const QString& fen)
{
    ChessEngine engine;
    engine.setPosition(fen);

    auto legalMoves = engine.getLegalMoves();
    if (legalMoves.empty()) {
        return "";
    }

    if (legalMoves.size() == 1) {
        return legalMoves[0].from + legalMoves[0].to;
    }

    auto scoredMoves = scoreAllMoves(fen, engine);

    std::sort(scoredMoves.begin(), scoredMoves.end(),
        [](const ScoredMove& a, const ScoredMove& b) {
            return a.score > b.score;
        }
    );

    if (scoredMoves.empty()) {
        return legalMoves[0].from + legalMoves[0].to;
    }

    int topScore = scoredMoves[0].score;
    std::vector<ScoredMove> topMoves;
    for (const auto& sm : scoredMoves) {
        if (sm.score >= topScore - 5) {
            topMoves.push_back(sm);
        } else {
            break;
        }
    }

    int randomIndex = std::rand() % topMoves.size();
    const auto& bestMove = topMoves[randomIndex].move;
    return bestMove.from + bestMove.to;
}

std::vector<AiOpponent::ScoredMove> AiOpponent::scoreAllMoves(const QString& fen, ChessEngine& engine)
{
    std::vector<ScoredMove> scoredMoves;
    auto legalMoves = engine.getLegalMoves();

    for (const auto& move : legalMoves) {
        ScoredMove sm;
        sm.move = move;
        sm.score = evaluateMove(move, engine);
        scoredMoves.push_back(sm);
    }

    return scoredMoves;
}

int AiOpponent::evaluateMove(const Move& move, ChessEngine& engine)
{
    int score = 0;

    if (isCaptureMove(move, engine)) {
        score += 50;
    }

    if (isCheckingMove(move, engine)) {
        score += 30;
    }

    char piece = move.from[0].toLatin1();
    if (piece >= 'a' && piece <= 'h') {
        int col = piece - 'a';
        if (col >= 2 && col <= 5) {
            score += 10;
        }
    }

    score += std::rand() % 20;

    return score;
}

int AiOpponent::evaluatePosition(const QString& fen)
{
    int score = 0;

    for (char c : fen.toLatin1()) {
        switch (c) {
        case 'P': score += 1; break;
        case 'N': case 'B': score += 3; break;
        case 'R': score += 5; break;
        case 'Q': score += 9; break;
        case 'p': score -= 1; break;
        case 'n': case 'b': score -= 3; break;
        case 'r': score -= 5; break;
        case 'q': score -= 9; break;
        case ' ': return score;
        default: break;
        }
    }

    return score;
}

bool AiOpponent::isCaptureMove(const Move& move, ChessEngine& engine)
{
    const auto& fen = engine.getFen();
    int toCol = move.to[0].toLatin1() - 'a';
    int toRow = 8 - (move.to[1].toLatin1() - '0');

    const QStringList parts = fen.split(' ');
    if (parts.isEmpty()) return false;

    const QString position = parts[0];
    const QStringList rows = position.split('/');

    if (toRow < 0 || toRow >= rows.size()) return false;

    int currentCol = 0;
    for (char c : rows[toRow].toLatin1()) {
        if (!std::isdigit(c)) {
            if (currentCol == toCol) {
                return c != 0 && c != ' ';
            }
            currentCol++;
        } else {
            currentCol += (c - '0');
        }
    }

    return false;
}

bool AiOpponent::isCheckingMove(const Move& move, ChessEngine& engine)
{
    ChessEngine testEngine = engine;
    testEngine.makeMove(move.from, move.to);
    return false;
}
