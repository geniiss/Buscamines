#include "game.hh"

void printBoard (int rows, int cols, std::string result) {
    for(int i = 0; i < rows; i++) {
      for(int j = 0; j < cols; j++) {
        std::cout << result[i * cols + j] << ' ';
      }
      std::cout << std::endl;
    }
  }

int main () {
  std::cout << "Enter the number of rows, cols and mines separated by space: ";
  int rows, cols, mines;
  std::cin >> rows >> cols >> mines;
  Game myGame(rows, cols, mines);

  std::string result = myGame.printBoard();
  printBoard(rows, cols, result);
  std::cout << "Enter the row and col separated by space: ";
  int i, j;
  std::cin >> i >> j;
  myGame.unveilCell(i, j);
  result = myGame.printBoard();
  printBoard(rows, cols, result);

  GameStates gameState = myGame.getGameState();
  while(gameState == PLAYING) {
    std::cout << "Enter the row and col separated by space: ";
    std::cin >> i >> j;
    myGame.unveilCell(i, j);
    result = myGame.printBoard();
    printBoard(rows, cols, result);
    gameState = myGame.getGameState();
  }

  std::cout << (gameState == WIN ? "You win!" : "You lose!") << std::endl;
}