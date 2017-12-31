// main.cc
#include <gameEngine/Game.h>

int main() {
  Game::init();
  while (Game::theOne().shouldRun()) {
    Game::theOne().run();
  }
  return 0;
}
