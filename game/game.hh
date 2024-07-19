#ifndef GAME_HH
#define GAME_HH

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stdlib.h> 
#include <time.h>
#include <string>
#include <iostream>

struct Squares {
  bool hasMine = false;
  bool isRevealed = false;
  bool isFlagged = false;
  int numAdjacentMines = 0;
};

typedef enum {
  READY,
  PLAYING,
  WIN,
  LOSE
} GameStates;

class Game {
  private:
    struct pair_hash {
      template <class T1, class T2>
      std::size_t operator() (const std::pair<T1, T2>& p) const {
          auto hash1 = std::hash<T1>{}(p.first);
          auto hash2 = std::hash<T2>{}(p.second);
          return hash1 ^ hash2;  // Combine the two hash values
      }
    };

    int rows;
    int cols;
    int total_mines;
    int flags;
    GameStates gameState = READY;
    std::vector<std::vector<Squares>> board;
    std::unordered_set<std::pair<int,int>, pair_hash> safeCells;
    std::unordered_set<std::pair<int,int>, pair_hash> mineCells;
    std::unordered_set<std::pair<int,int>, pair_hash> uncertainCells; 
    std::vector<std::vector<int>> probabilites;
    std::vector<std::pair<int, int>> indexPos; //maps the index with the board position
    std::unordered_map<std::pair<int, int>, int, pair_hash> posIndex; //maps the position with the index


    //auxiliar vectors to iterate over the adjacent cells
    std::vector<int> adjI = {-1, -1, -1, 0, 0, 1, 1, 1};
    std::vector<int> adjJ = {1, 0, -1, 1, -1, 1, 0, -1};

    bool pos_ok (int i , int j);
    void unveilCellRecursive (int i, int j);
    void unveilCell(int i, int j);
    void distributeMines (int i, int j);
    bool checkWin ();

    void computeProbabilities ();
    void computeProbabilitiesBT (int n, const std::vector<std::vector<int>>& indexCond, const std::vector<std::vector<int>>& conditions, std::vector<int>& currentDisp, std::vector<std::vector<int>>& results, int currentMines);
    std::vector<std::pair<int, int>> adjacentCoveredCells(int, int);
    void redistributeMines (int, int);

  public:
    Game (int r, int c, int m);
    void markCell (int i, int j);
    void clickCell (int i, int j);
    std::string printBoard ();
    int getGameState();
};

#endif