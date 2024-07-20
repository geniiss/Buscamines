#include <emscripten/bind.h>
#include "game.hh"

EMSCRIPTEN_BINDINGS(my_class_example) {
    emscripten::class_<Game>("Game")
        .constructor<int, int, int>()
        .function("rightClickCell", &Game::rightClickCell)
        .function("clickCell", &Game::clickCell)
        .function("printBoard", &Game::printBoard)
        .function("getRemainingMines", &Game::getRemainingMines)
        .function("getGameState", &Game::getGameState);
}
