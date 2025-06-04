// Players.cpp
#include "GameCore.h"
#include <iostream>
#include <limits>
#include <algorithm>
#include <climits>

using namespace std;
static const int INF = numeric_limits<int>::max() / 2;

//––– HumanPlayer –––––––––––––––––––––––––––––––––––––––––––––––––––––––––
int HumanPlayer::chooseMove(const Scaffold& s, int N, int color) {
    if (s.numberEmpty() == 0) return 0;
    int col = 0;
    while (true) {
        cout << "[" << m_name << "] Select column (1–"
             << s.cols() << "): ";
        if (!(cin >> col) || col < 1 || col > s.cols()
            || s.checkerAt(col, s.levels()) != VACANT) {
            cin.clear();
            cin.ignore(1e6,'\n');
            cout << "Invalid. Try again.\n";
            continue;
        }
        break;
    }
    return col;
}

//––– BadPlayer ––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
int BadPlayer::chooseMove(const Scaffold& s, int N, int color) {
    for (int c = 1; c <= s.cols(); c++)
        if (s.checkerAt(c, 1) == VACANT)
            return c;
    return 0;
}

//––– SmartPlayer ––––––––––––––––––––––––––––––––––––––––––––––––––––––––
int SmartPlayer::chooseMove(const Scaffold& s, int N, int color) {
    AlarmClock ac(8750);  // Time-limited search
    return findBestMove(s, N, color, ac);
}

int SmartPlayer::findBestMove(const Scaffold& s, int N, int color, AlarmClock& ac) {
    int opp = (color == RED ? BLACK : RED);
    int bestScore = -INF, alpha = -INF, beta = +INF;
    vector<int> bestMoves;
    Scaffold scaf = s;

    for (int c = 1; c <= s.cols(); c++) {
        if (ac.timedOut()) break;
        if (!scaf.makeMove(c, color)) continue;

        int score = -miniMax(scaf, N, opp, 1, -beta, -alpha, ac);
        scaf.undoMove();

        if (score > bestScore) {
            bestScore = score;
            bestMoves = {c};
        } else if (score == bestScore) {
            bestMoves.push_back(c);
        }

        alpha = max(alpha, bestScore);
    }

    if (bestMoves.empty()) return 0;
    return bestMoves.front();  // Could randomize for variety
}

int SmartPlayer::miniMax(Scaffold& s, int N, int color, int depth,
                         int alpha, int beta, AlarmClock& ac) {
    if (ac.timedOut()) return 0;

    int eval = evaluateState(s, N, color, depth, ac);
    if (eval != INT_MIN) return eval;

    int opp = (color == RED ? BLACK : RED);
    int best = -INF;

    for (int c = 1; c <= s.cols(); c++) {
        if (ac.timedOut()) break;
        if (!s.makeMove(c, color)) continue;

        int score = -miniMax(s, N, opp, depth + 1, -beta, -alpha, ac);
        s.undoMove();

        best = max(best, score);
        alpha = max(alpha, best);
        if (alpha >= beta) break;  // Alpha-beta pruning
    }

    return best;
}

int SmartPlayer::evaluateState(const Scaffold& s, int N, int color, int depth, AlarmClock& ac) {
    if (ac.timedOut()) return 0;

    GameState gs = checkState(s, N);
    if (gs == PLAYING) {
        // Heuristic evaluation for non-terminal state
        return heuristicScore(s, N, color);
    }

    if ((gs == REDWIN && color == RED) || (gs == BLACKWIN && color == BLACK))
        return 100000 - depth;
    if ((gs == REDWIN && color == BLACK) || (gs == BLACKWIN && color == RED))
        return -100000 + depth;

    return 0;  // TIE
}

int SmartPlayer::heuristicScore(const Scaffold& s, int N, int color) {
    int opp = (color == RED ? BLACK : RED);

    // Pre-compute current height of each column to reason about reachability of
    // cells above the existing stack.  A cell is considered reachable only if
    // its row index is at most height[col] + 1.
    std::vector<int> height(s.cols() + 1, 0);
    for (int c = 1; c <= s.cols(); ++c) {
        for (int r = s.levels(); r >= 1; --r) {
            if (s.checkerAt(c, r) != VACANT) { height[c] = r; break; }
        }
    }

    auto scoreLine = [&](int c, int r, int dc, int dr, int col) {
        int countColor = 0, countEmpty = 0;
        for (int i = 0; i < N; ++i) {
            int cc = c + i * dc, rr = r + i * dr;
            if (cc < 1 || cc > s.cols() || rr < 1 || rr > s.levels()) return 0;
            int cell = s.checkerAt(cc, rr);
            if (cell == col) {
                countColor++;
            } else if (cell == VACANT) {
                // If this cell is not immediately reachable, the pattern is
                // unlikely to be completed soon, so disregard this line.
                if (rr > height[cc] + 1) return 0;
                countEmpty++;
            } else {
                // Blocked by the opponent
                return 0;
            }
        }
        return (countColor == 0 ? 0 : (countColor * countColor + countEmpty));
    };

    int score = 0;
    const vector<pair<int,int>> dirs = {{1,0},{0,1},{1,1},{1,-1}};
    for (int c = 1; c <= s.cols(); ++c) {
        for (int r = 1; r <= s.levels(); ++r) {
            for (auto [dc, dr] : dirs) {
                score += scoreLine(c, r, dc, dr, color);
                score -= scoreLine(c, r, dc, dr, opp);
            }
        }
    }
    return score;
}

GameState SmartPlayer::checkState(const Scaffold& s, int N) {
    auto inBounds = [&](int c, int r) {
        return c >= 1 && c <= s.cols() && r >= 1 && r <= s.levels();
    };
    const vector<pair<int,int>> dirs = {{1,0},{0,1},{1,1},{1,-1}};

    for (int c = 1; c <= s.cols(); ++c) {
        for (int r = 1; r <= s.levels(); ++r) {
            int col = s.checkerAt(c, r);
            if (col == VACANT) continue;
            for (auto [dc, dr] : dirs) {
                int count = 1;
                int nc = c + dc, nr = r + dr;
                while (inBounds(nc, nr) && s.checkerAt(nc, nr) == col) {
                    count++; nc += dc; nr += dr;
                }
                if (count >= N)
                    return col == RED ? REDWIN : BLACKWIN;
            }
        }
    }
    return s.numberEmpty() == 0 ? TIE : PLAYING;
}
