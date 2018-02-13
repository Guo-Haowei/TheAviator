// Game.h
#pragma once
#include <renderEngine/Renderer.h>

class Game {
private:
  Renderer renderer;
  
  double currentTime, lastTime;
  double previousSecond = lastTime;
  double delta = 0;
  int updates = 0;
public:
  Game();
  ~Game();

  void run();
  bool shouldRun();
  bool shouldUpdate();
  static void init();
  static Game& theOne();
};
