#ifndef GAME_HH
#define GAME_HH

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
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

    const int rows;
    const int cols;
    const int total_mines;
    int remaining_mines;
    bool discoverMode = false;
    static const int chunkSize = 25;
    GameStates gameState = READY;
    std::vector<std::vector<Squares>> board;
    std::unordered_set<std::pair<int,int>, pair_hash> safeCells;
    std::unordered_set<std::pair<int,int>, pair_hash> mineCells;
    std::unordered_set<std::pair<int,int>, pair_hash> uncertainCells; 
    std::vector<std::vector<std::vector<int>>> probabilites;
    std::vector<std::vector<std::pair<int, int>>> indexPos; //maps the index with the board position
    std::vector<std::unordered_map<std::pair<int, int>, int, pair_hash>> posIndex; //maps the position with the index
    std::vector<std::vector<std::vector<int>>> conditions; //the first element of each vector is how many mines should its adjacent cells have, and the rest of the elements have the indices of its adjacent cells
    std::vector<std::vector<std::vector<int>>> indexCond; //each vector contains every condition index that is in contact with the cell of each index


    //auxiliar vectors to iterate over the adjacent cells
    std::vector<int> adjI = {-1, -1, -1, 0, 0, 1, 1, 1};
    std::vector<int> adjJ = {1, 0, -1, 1, -1, 1, 0, -1};

    bool pos_ok (int i , int j);
    void unveilCellRecursive (int i, int j);
    void unveilCell(int i, int j);
    void markCell(int i, int j);
    void distributeMines (int i, int j);
    bool checkWin ();

    void computeProbabilities ();
    void computeProbabilitiesBT (int n, const std::vector<std::vector<int>>& indexCond, const std::vector<std::vector<int>>& conditions, std::vector<int>& currentDisp, std::vector<std::vector<int>>& results, int currentMines);
    std::vector<std::pair<int, int>> adjacentCoveredCells(int, int);
    std::vector<std::pair<int, int>> adjacentUncoveredCells(int, int);
    void redistributeMines (int, int, bool = true);

  public:
    Game (int r, int c, int m, bool mode);
    void rightClickCell (int i, int j);
    void clickCell (int i, int j);
    std::string printBoard ();
    int getGameState();
    int getRemainingMines();
    void setGameMode(bool mode);
};

#endif