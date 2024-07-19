#include <emscripten/bind.h>
#include "game.hh"

EMSCRIPTEN_BINDINGS(my_class_example) {
    emscripten::class_<Game>("Game")
        .constructor<int, int, int>()
        .function("markCell", &Game::markCell)
        .function("clickCell", &Game::clickCell)
        .function("printBoard", &Game::printBoard)
        .function("getGameState", &Game::getGameState);
}
