#include "game.hh"
#include <unordered_map>
#include <unordered_set>

std::vector<std::pair<int,int>> Game::adjacentCoveredCells(int i, int j) {
  std::vector<std::pair<int,int>> result;
  for (int k = 0; k < adjI.size(); ++k) {
    int posI = i+adjI[k];
    int posJ = j+adjJ[k];
    if (!pos_ok(posI, posJ) || board[posI][posJ].isRevealed) continue;
    result.push_back(std::make_pair(posI, posJ));
  }
  return result;
}

void Game::computeProbabilitiesBT (int n, const std::vector<std::vector<int>>& indexCond, const std::vector<std::vector<int>>& conditions, std::vector<int>& currentDisp, std::vector<std::vector<int>>& results, int currentMines) {
  if (currentMines > total_mines) return;
  int lastAddedCellIdx = currentDisp.size()-1; 
  int m = (lastAddedCellIdx == -1 ? 0 : indexCond[lastAddedCellIdx].size());
  for (int i = 0; i < m; ++i) {
    int condIdx = indexCond[lastAddedCellIdx][i];
    int maxMinesCond = conditions[condIdx][0];
    int minesCount = 0;
    int notAddedCells = 0;
    for(int j = 1; j < conditions[condIdx].size(); ++j) {
      if(conditions[condIdx][j] >= currentDisp.size()) {
        ++notAddedCells;
        continue;
      }
      if(currentDisp[conditions[condIdx][j]]) ++minesCount;
      if(minesCount > maxMinesCond) return;
    }
    if (!notAddedCells && minesCount < maxMinesCond) return;
  }

  if (currentDisp.size() == n) {
    results.push_back(currentDisp);
    return;
  }
  //condicions comprovades
  currentDisp.push_back(1);
  computeProbabilitiesBT(n, indexCond, conditions, currentDisp, results, currentMines+1);
  currentDisp.pop_back();
  currentDisp.push_back(0);
  computeProbabilitiesBT(n, indexCond, conditions, currentDisp, results, currentMines);
  currentDisp.pop_back();
}

void Game::computeProbabilities () {
  indexPos.clear(); //maps the index with the board position
  posIndex.clear(); //maps the position with the index
  std::vector<std::vector<int>> conditions; //the first element of each vector is how many mines should its adjacent cells have, and the rest of the elements have the indices of its adjacent cells
  std::vector<std::vector<int>> indexCond; //each vector contains every condition index that is in contact with the cell of each index

  //loop to find conditions
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      if (board[i][j].isRevealed && board[i][j].numAdjacentMines > 0) {
        std::vector<int> condition = {board[i][j].numAdjacentMines};
        std::vector<std::pair<int, int>> cellsToAdd = adjacentCoveredCells(i, j);
        for (int k = 0; k < cellsToAdd.size(); ++k) {
          auto it = posIndex.find(cellsToAdd[k]);
          int index = -1;
          if (it != posIndex.end()) {
            index = it->second;
            indexCond[index].push_back(conditions.size());
          }
          else {
            indexCond.push_back(std::vector<int>(1, conditions.size()));
            indexPos.push_back(cellsToAdd[k]);
            index = indexPos.size()-1;
            posIndex[cellsToAdd[k]] = index;
          }
          condition.push_back(index);
        }
        conditions.push_back(condition);
      }
    }
  }

  probabilites.clear();
  std::vector<int> currentDisp;

  computeProbabilitiesBT(indexPos.size(), indexCond, conditions, currentDisp, probabilites, 0);

  std::cout << std::endl << "Possibles solucions: " << std::endl;

  for (int i = 0; i < probabilites.size(); ++i) {
    for (int j = 0; j < probabilites[i].size(); ++j) {
      std::cout << probabilites[i][j] << ' ';
    }
    std::cout << std::endl;
  }

  std::unordered_set<int> safePos;
  std::unordered_set<int> minePos;
  std::unordered_set<int> uncertainPos;

  for (int i = 0; i < indexPos.size(); ++i) {
    safePos.insert(i);
    minePos.insert(i);
  }

  for (int i = 0; i < probabilites.size(); ++i) {
    for (int j = 0; j < probabilites[i].size(); ++j) {
      if(probabilites[i][j]) {
        auto it = safePos.find(j);
        if (it != safePos.end()) safePos.erase(it);
      }
      else {
        auto it = minePos.find(j);
        if (it != minePos.end()) minePos.erase(it);
      }
      if (probabilites[i][j] != probabilites[0][j]) {
        auto it = uncertainPos.find(j);
        if (it == uncertainPos.end()) uncertainPos.insert(j);
      }
    }
  }
  safeCells.clear();
  for (int idxCell : safePos) {
    safeCells.insert(indexPos[idxCell]);
  }
  mineCells.clear();
  for (int idxCell : minePos) {
    mineCells.insert(indexPos[idxCell]);
  }
  uncertainCells.clear();
  for (int idxCell : uncertainPos) {
    uncertainCells.insert(indexPos[idxCell]);
  }

  std::cout << "Cells in which there cannot be a mine are:";
  for (const std::pair<int,int>& pos : safeCells) {
    std::cout << ' ' << pos.first << ',' << pos.second;
  }
  std::cout << std::endl;
}

void Game::redistributeMines (int posI, int posJ) {
  //clean whole board taking account the covered/uncovered and marked/unmarked
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      board[i][j].hasMine = false;
      board[i][j].numAdjacentMines = 0;
    }
  }
  
  std::vector<int> desiredLayout;
  if (posIndex.find(std::make_pair(posI,posJ)) != posIndex.end()) {
    for (int i = 0; i < probabilites.size(); ++i) {
      int idx = posIndex[std::make_pair(posI,posJ)];
      if (!probabilites[i][idx]) {
        desiredLayout = probabilites[i];
        break;
      }
    }
  }
  else desiredLayout = probabilites[0];
  //traverse the probabilities and pick one that doesn't have mine in position i j

  //force this layout
  int distributed_mines = 0;
  for (int i = 0; i < desiredLayout.size(); ++i) {
    if (desiredLayout[i]) {
      ++distributed_mines;
      std::pair<int,int> pos = indexPos[i];
      int i1 = pos.first;
      int j1 = pos.second;
      board[i1][j1].hasMine = true;
      for (int k = 0; k < adjI.size(); ++k){
        int posI = i1+adjI[k];
        int posJ = j1+adjJ[k];
        if (pos_ok(posI, posJ)) ++board[posI][posJ].numAdjacentMines;
      }
    }
  }

  //position rest of the mines:
  //first traverse the board finding available position for the mines
  std::vector<std::pair<int,int>> availablePos;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      if ((i == posI && j == posJ) || board[i][j].isRevealed || posIndex.find(std::make_pair(i, j)) != posIndex.end()) continue;
      availablePos.push_back(std::make_pair(i,j));
    }
  }

  //then use this array to distribute the mines randomly
  srand(time(NULL));
  while (availablePos.size() > 0 && distributed_mines < total_mines) {
    std::cout << 'a' << std::endl;
    int idx = rand() % availablePos.size();
    int i1 = availablePos[idx].first;
    int j1 = availablePos[idx].second;
    board[i1][j1].hasMine = true;
    for (int k = 0; k < adjI.size(); ++k){
      int posI = i1+adjI[k];
      int posJ = j1+adjJ[k];
      if (pos_ok(posI, posJ)) ++board[posI][posJ].numAdjacentMines;
    }
    availablePos.erase(std::next(availablePos.begin(), idx));
    ++distributed_mines;
  }
}