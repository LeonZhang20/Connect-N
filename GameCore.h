// GameCore.h
#ifndef GAMECORE_H
#define GAMECORE_H

#include <string>
#include <vector>
#include <chrono>
#include <iostream>

//––– Basic types & enums ––––––––––––––––––––––––––––––––––––––––––––––––––
enum Color     { VACANT = 0, RED = 1, BLACK = 2 };
enum GameState { PLAYING, REDWIN, BLACKWIN, TIE };

//––– AlarmClock: simple timeout guard ––––––––––––––––––––––––––––––––––––
class AlarmClock {
  public:
    // timeout in milliseconds
    AlarmClock(int timeoutMs)
      : m_deadline(std::chrono::steady_clock::now()
                   + std::chrono::milliseconds(timeoutMs)) {}
    bool timedOut() const {
      return std::chrono::steady_clock::now() > m_deadline;
    }
  private:
    std::chrono::steady_clock::time_point m_deadline;
};

//––– Scaffold: holds the board & move history for undo ––––––––––––––––––––
class Scaffold {
  public:
    Scaffold(int columns, int levels)
      : m_cols(columns), m_levels(levels),
        m_board(columns+1, std::vector<int>(levels+1, VACANT)), height(columns+1, 0)
    {}

    void display() const {

        for (int r = m_levels; r >= 1; --r) {
            for (int c = 1; c <= m_cols; ++c) {
                int v = checkerAt(c, r);
                char ch = v == VACANT ? '.' : (v == RED ? 'R' : 'B');
            std::cout << ch << ' ';
            }
        std::cout << '\n';
        }
    // column numbers
        for (int c = 1; c <= m_cols; ++c)
            std::cout << c << ' ';
        std::cout << "\n\n";  
   }

    // Attempt to drop into column; returns false if full/invalid
    bool makeMove(int col, int color) {
      if (col < 1 || col > m_cols)      return false;
      int h = height[col];
      if (h >= m_levels)                return false;
      m_board[col][++height[col]] = color;
      m_moves.push_back(col);
      return true;
    }

    // Undo the last move
    void undoMove() {
      if (m_moves.empty()) return;
      int col = m_moves.back();
      m_moves.pop_back();
      m_board[col][height[col]--] = VACANT;
    }

    // Inspect a cell
    int checkerAt(int col, int level) const {
      if (col < 1 || col > m_cols ||
          level < 1 || level > m_levels)
        return VACANT;
      return m_board[col][level];
    }

    int cols()        const { return m_cols; }
    int levels()      const { return m_levels; }
    int numberEmpty() const {
      return m_cols * m_levels
             - static_cast<int>(m_moves.size());
    }

  private:
    int                              m_cols, m_levels;
    std::vector<std::vector<int>>    m_board;
    std::vector<int>                 height; 
    std::vector<int>                 m_moves;
};

//––– Abstract Player –––––––––––––––––––––––––––––––––––––––––––––––––––––
class Player {
  public:
    Player(const std::string& nm) : m_name(nm) {}
    virtual ~Player() {}
    virtual int  chooseMove(const Scaffold& s, int N, int color) = 0;
    virtual bool isInteractive() const = 0;
    std::string name() const { return m_name; }
  protected:
    std::string m_name;
};

class HumanPlayer : public Player {
  public:
    HumanPlayer(const std::string& nm) : Player(nm) {}
    int  chooseMove(const Scaffold& s, int N, int color) override;
    bool isInteractive() const override { return true; }
};

class BadPlayer : public Player {
  public:
    BadPlayer(const std::string& nm) : Player(nm) {}
    int  chooseMove(const Scaffold& s, int N, int color) override;
    bool isInteractive() const override { return false; }
};

class SmartPlayer : public Player {
  public:
    SmartPlayer(const std::string& nm) : Player(nm) {}
    int  chooseMove(const Scaffold& s, int N, int color) override;
    bool isInteractive() const override { return false; }

  private:
    int  findBestMove(const Scaffold& s, int N, int color, AlarmClock& ac);
    int  miniMax(Scaffold& s, int N, int color, int depth,
                 int alpha, int beta, AlarmClock& ac);
    int  evaluateState(const Scaffold& s, int N,
                       int color, int depth, AlarmClock& ac);
    int  heuristicScore(const Scaffold& s, int N, int color);
    GameState checkState(const Scaffold& s, int N);
};

class GameImpl;  // defined in Game.cpp

//-- Game facade that uses a private implementation -------------------------
class Game {
  public:
    Game(int nColumns, int nLevels, int N,
         Player* red, Player* black);
    ~Game();
    bool completed(int& winner) const;
    bool takeTurn();
    void play();
    int  checkerAt(int c, int r) const;

  private:
    GameImpl* m_impl;
};

#endif // GAMECORE_H
