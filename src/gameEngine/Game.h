// Game.h
#pragma once
#include <renderEngine/Renderer.h>
#include <entities/gameObjects/Airplane.h>
#include <entities/gameObjects/Sky.h>
#include <entities/gameObjects/ObstacleHolder.h>

class Game {
private:
  Renderer renderer;
  Airplane airplane;
  Sky sky;
  ObstacleHolder obstacles;
  
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
