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
  if(board[i][j].isFlagged) return;
  if (gameState == READY) {
    gameState = PLAYING;
    distributeMines(i, j);
  }
  computeProbabilities();
  if (board[i][j].hasMine) {
    if (safeCells.empty() && mineCells.find(std::make_pair(i,j)) == mineCells.end()) redistributeMines(i, j);
    else {
      gameState = LOSE;
      return;
    }
  }
  unveilCellRecursive(i, j);
  if (checkWin()) gameState = WIN;
}

void Game::markCell (int i, int j) {
  if (board[i][j].isRevealed) return;
  if (board[i][j].isFlagged) ++remaining_mines;
  else --remaining_mines;
  board[i][j].isFlagged = !board[i][j].isFlagged;
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

Game::Game (int r, int c, int m, bool mode) : rows(r), cols(c), total_mines(m) {
  this->remaining_mines = m;
  this->discoverMode = mode;
  this->board = std::vector<std::vector<Squares>>(r, std::vector<Squares>(c));
}

void Game::rightClickCell (int i, int j) {
  if (!pos_ok(i,j)) return;
  if (!discoverMode) {
    unveilCell(i,j);
    return;
  }
  markCell(i, j);
}

void Game::clickCell (int i, int j) {
  if (!pos_ok(i, j)) return;
  if (gameState == READY) {
    unveilCell(i, j);
    return;
  }
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
  if (!discoverMode) {
    markCell(i, j);
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

int Game::getRemainingMines() {
  return remaining_mines;
}

void Game::setGameMode(bool mode) {
  discoverMode = mode;
}