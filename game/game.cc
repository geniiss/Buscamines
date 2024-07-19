#include "game.hh"

bool Game::pos_ok (int i , int j) { 
  return i >= 0 && i < rows && j >= 0 && j < cols; 
}

void Game::unveilCellRecursive (int i, int j) {
  if (board[i][j].isRevealed) return;
  board[i][j].isRevealed = true;
  if (board[i][j].numAdjacentMines != 0) return;
  for (int k = 0; k < adjI.size(); ++k) {
    int posI = i + adjI[k];
    int posJ = j + adjJ[k];
    if (!pos_ok(posI, posJ)) continue;
    unveilCellRecursive(posI, posJ);
  }
}

void Game::unveilCell(int i, int j) {
  if (gameState == READY) {
    gameState = PLAYING;
    distributeMines(i, j);
  }
  computeProbabilities();
  if (board[i][j].hasMine) {
    if (safeCells.empty() && uncertainCells.find(std::make_pair(i,j)) != uncertainCells.end()) redistributeMines(i, j);
    else {
      gameState = LOSE;
      return;
    }
  }
  unveilCellRecursive(i, j);
  if (checkWin()) gameState = WIN;
}

void Game::distributeMines (int i, int j) {
  srand(time(NULL));
  int distributed_mines = 0;
  while (distributed_mines < total_mines) {
    int i1 = rand() % rows;
    int j1 = rand() % cols;
    //If the random generated cell has already a mine or it's in between the first clicked 3*3 area, continue
    if (board[i1][j1].hasMine || (i1 <= i + 1 && j1 <= j + 1 &&  i1 >= i - 1 && j1 >= j - 1)) continue;
    board[i1][j1].hasMine = true;
    for (int k = 0; k < adjI.size(); ++k){
      int posI = i1+adjI[k];
      int posJ = j1+adjJ[k];
      if (pos_ok(posI, posJ)) ++board[posI][posJ].numAdjacentMines;
    }
    ++distributed_mines;
  }
}

bool Game::checkWin () {
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      if (!board[i][j].hasMine && !board[i][j].isRevealed) {
        return false;
      }
    }
  }
  return true;
}

Game::Game (int r, int c, int m) {
  this->rows = r;
  this->cols = c;
  this->total_mines = m;
  this->flags = 0;
  this->board = std::vector<std::vector<Squares>>(r, std::vector<Squares>(c));
}

void Game::markCell (int i, int j) {
  board[i][j].isFlagged = true;
}

void Game::clickCell (int i, int j) {
  if (!pos_ok(i, j) || board[i][j].isFlagged) return;
  if (board[i][j].isRevealed) {
    int adjacentMines = board[i][j].numAdjacentMines;
    int currentAdjMarked = 0;
    for (int k = 0; k < adjI.size(); ++k) {
      int posI = i + adjI[k];
      int posJ = j + adjJ[k];
      if(pos_ok(posI, posJ) && board[posI][posJ].isFlagged) ++currentAdjMarked;
    }
    if(adjacentMines == currentAdjMarked) {
      for (int k = 0; k < adjI.size(); ++k) {
        int posI = i + adjI[k];
        int posJ = j + adjJ[k];
        if(pos_ok(posI, posJ) && !board[posI][posJ].isRevealed && !board[posI][posJ].isFlagged) unveilCell(posI, posJ);
      }
    }
    return;
  }
  unveilCell(i, j);
}

std::string Game::printBoard () {
  std::string res;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      if (gameState == LOSE) {
        if (board[i][j].isFlagged){
          if (board[i][j].hasMine) res.push_back('e');
          else res.push_back('f');
        }
        else if (board[i][j].hasMine) res.push_back('m');
        else if (board[i][j].isRevealed) res.push_back(char(board[i][j].numAdjacentMines + '0'));
        else res.push_back('c');
      }
      else {
        if (board[i][j].isRevealed) res.push_back(char(board[i][j].numAdjacentMines + '0'));
        else if (board[i][j].isFlagged) res.push_back('f');
        else res.push_back('c');
      }
    }
  }
  return res;
}

int Game::getGameState(){
  return gameState;
}