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
  std::vector<std::vector<std::vector<int>>> conditions; //the first element of each vector is how many mines should its adjacent cells have, and the rest of the elements have the indices of its adjacent cells
  std::vector<std::vector<std::vector<int>>> indexCond; //each vector contains every condition index that is in contact with the cell of each index
  /*
  * for cell of idx 0, it is in contact with conditions 5 3 and 2
  * and for cell of idx 1, it is in contact with conditions 3, 2 and 1
  * [[5, 3, 2],
  * [3, 2, 1]]
  */

  std::unordered_set<std::pair<int, int>, pair_hash> addedConds;

  //loop to find conditions
  int idxSituation = 0; //we are going to study separate situations
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      if (board[i][j].isRevealed && board[i][j].numAdjacentMines > 0 && addedConds.find(std::make_pair(i, j)) == addedConds.end()) {

        indexPos.push_back(std::vector<std::pair<int, int>>());
        posIndex.push_back(std::unordered_map<std::pair<int, int>, int, pair_hash>());
        conditions.push_back(std::vector<std::vector<int>>());
        indexCond.push_back(std::vector<std::vector<int>>());

        //start of a bfs
        std::queue<std::pair<int, int>> cellsToStudy;
        cellsToStudy.push(std::make_pair(i, j));
        while (!cellsToStudy.empty() && indexPos[idxSituation].size() < 10) {
          int i1 = cellsToStudy.front().first;
          int j1 = cellsToStudy.front().second;
          cellsToStudy.pop();
          addedConds.insert(std::make_pair(i1, j1));

          std::vector<int> condition = {board[i1][j1].numAdjacentMines};
          std::vector<std::pair<int, int>> cellsToAdd = adjacentCoveredCells(i1, j1);
          for (int k = 0; k < cellsToAdd.size(); ++k) {
            auto it = posIndex[idxSituation].find(cellsToAdd[k]);
            int index = -1;
            if (it != posIndex[idxSituation].end()) {
              index = it->second;
              indexCond[idxSituation][index].push_back(conditions[idxSituation].size());
            }
            else {
              indexCond[idxSituation].push_back(std::vector<int>(1, conditions[idxSituation].size()));
              indexPos[idxSituation].push_back(cellsToAdd[k]);
              index = indexPos[idxSituation].size()-1;
              posIndex[idxSituation][cellsToAdd[k]] = index;
            }
            condition.push_back(index);
          }
          conditions[idxSituation].push_back(condition);
          
          for (int k = 0; k < adjI.size(); ++k) {
            int i2 = i1 + adjI[k];
            int j2 = j1 + adjJ[k];

            if (pos_ok(i2, j2) && board[i2][j2].isRevealed && board[i2][j2].numAdjacentMines > 0 && addedConds.find(std::make_pair(i2, j2)) == addedConds.end()) {
              cellsToStudy.push(std::make_pair(i2, j2));
            }
          }
        }
        ++idxSituation;
      }
    }
  }

  probabilites.clear();
  
  for (int k = 0; k < indexPos.size(); ++k) {
    std::vector<int> currentDisp;
    probabilites.push_back(std::vector<std::vector<int>>());
    computeProbabilitiesBT(indexPos[k].size(), indexCond[k], conditions[k], currentDisp, probabilites[k], 0);
  }

  std::vector<std::unordered_set<int>> safePos(probabilites.size(), std::unordered_set<int>());
  std::vector<std::unordered_set<int>> minePos(probabilites.size(), std::unordered_set<int>());
  std::vector<std::unordered_set<int>> uncertainPos(probabilites.size(), std::unordered_set<int>());

  for (int i = 0; i < indexPos.size(); ++i) {
    for (int j = 0; j < indexPos[i].size(); ++j) {
      safePos[i].insert(j);
      minePos[i].insert(j);
    }
  }

  for (int i = 0; i < probabilites.size(); ++i) {
    for (int j = 0; j < probabilites[i].size(); ++j) {
      for (int k = 0; k < probabilites[i][j].size(); ++k) {
        if(probabilites[i][j][k]) {
          auto it = safePos[i].find(k);
          if (it != safePos[i].end()) safePos[i].erase(it);
        }
        else {
          auto it = minePos[i].find(k);
          if (it != minePos[i].end()) minePos[i].erase(it);
        }
        if (probabilites[i][j][k] != probabilites[i][0][k]) {
          auto it = uncertainPos[i].find(k);
          if (it == uncertainPos[i].end()) uncertainPos[i].insert(k);
        }
      }
    }
  }

  safeCells.clear();
  uncertainCells.clear();
  mineCells.clear();

  for (int i = 0; i < safePos.size(); ++i) {
    for (int idxCell : safePos[i])
      safeCells.insert(indexPos[i][idxCell]);
  }

  for (int i = 0; i < minePos.size(); ++i) {
    for (int idxCell : minePos[i]) {
      std::pair<int,int> pos = indexPos[i][idxCell];
      auto it = safeCells.find(pos);
      if (it == safeCells.end()) mineCells.insert(pos);
      else {
        safeCells.erase(it);
        uncertainCells.insert(pos);
      }
    }
  }

  for (int i = 0; i < uncertainPos.size(); ++i) {
    for (int idxCell : uncertainPos[i]) {
      std::pair<int,int> pos = indexPos[i][idxCell];
      auto it = uncertainCells.find(pos);
      if (it == uncertainCells.end()) uncertainCells.insert(pos);
    }
  }
  

  std::cout << "Cells in which there cannot be a mine are:";
  for (const std::pair<int,int>& pos : safeCells) {
    std::cout << ' ' << pos.first << ',' << pos.second;
  }
  std::cout << std::endl;

  std::cout << "Cells in which there is a mine are:";
  for (const std::pair<int,int>& pos : mineCells) {
    std::cout << ' ' << pos.first << ',' << pos.second;
  }
  std::cout << std::endl;

  std::cout << "Cells in which could be a mine are:";
  for (const std::pair<int,int>& pos : uncertainCells) {
    std::cout << ' ' << pos.first << ',' << pos.second;
  }
  std::cout << std::endl;
}

void Game::redistributeMines (int posI, int posJ) {
  //clean whole board taking account the covered/uncovered and marked/unmarked
  // for (int i = 0; i < rows; ++i) {
  //   for (int j = 0; j < cols; ++j) {
  //     board[i][j].hasMine = false;
  //     board[i][j].numAdjacentMines = 0;
  //   }
  // }
  
  // std::vector<int> desiredLayout;
  // if (posIndex.find(std::make_pair(posI,posJ)) != posIndex.end()) {
  //   for (int i = 0; i < probabilites.size(); ++i) {
  //     int idx = posIndex[std::make_pair(posI,posJ)];
  //     if (!probabilites[i][idx]) {
  //       desiredLayout = probabilites[i];
  //       break;
  //     }
  //   }
  // }
  // else desiredLayout = probabilites[0];
  // //traverse the probabilities and pick one that doesn't have mine in position i j

  // //force this layout
  // int distributed_mines = 0;
  // for (int i = 0; i < desiredLayout.size(); ++i) {
  //   if (desiredLayout[i]) {
  //     ++distributed_mines;
  //     std::pair<int,int> pos = indexPos[i];
  //     int i1 = pos.first;
  //     int j1 = pos.second;
  //     board[i1][j1].hasMine = true;
  //     for (int k = 0; k < adjI.size(); ++k){
  //       int posI = i1+adjI[k];
  //       int posJ = j1+adjJ[k];
  //       if (pos_ok(posI, posJ)) ++board[posI][posJ].numAdjacentMines;
  //     }
  //   }
  // }

  // //position rest of the mines:
  // //first traverse the board finding available position for the mines
  // std::vector<std::pair<int,int>> availablePos;
  // for (int i = 0; i < rows; ++i) {
  //   for (int j = 0; j < cols; ++j) {
  //     if ((i == posI && j == posJ) || board[i][j].isRevealed || posIndex.find(std::make_pair(i, j)) != posIndex.end()) continue;
  //     availablePos.push_back(std::make_pair(i,j));
  //   }
  // }

  // //then use this array to distribute the mines randomly
  // srand(time(NULL));
  // while (availablePos.size() > 0 && distributed_mines < total_mines) {
  //   std::cout << 'a' << std::endl;
  //   int idx = rand() % availablePos.size();
  //   int i1 = availablePos[idx].first;
  //   int j1 = availablePos[idx].second;
  //   board[i1][j1].hasMine = true;
  //   for (int k = 0; k < adjI.size(); ++k){
  //     int posI = i1+adjI[k];
  //     int posJ = j1+adjJ[k];
  //     if (pos_ok(posI, posJ)) ++board[posI][posJ].numAdjacentMines;
  //   }
  //   availablePos.erase(std::next(availablePos.begin(), idx));
  //   ++distributed_mines;
  // }
}