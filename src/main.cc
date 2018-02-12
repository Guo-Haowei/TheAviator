// main.cc
#include <common.h>
#include <utils/Maths.h>
#include <entities/gameObjects/Camera.h>
#include <entities/Entity.h>
#include <entities/gameObjects/Airplane.h>
#include <entities/gameObjects/Sky.h>
#include <entities/gameObjects/ObstacleHolder.h>
#include <models/Geometry.h>
#include <renderEngine/DisplayManager.h>
#include <renderEngine/Renderer.h>
#include <io/Parser.h>
#include <io/MouseManager.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
using std::cout;
using std::vector;

float GAME::AIRPLANE_DISTANCE = 0.0f;

float TIMER = 0;

bool shouldUpdate(double& currentTime, double& delta, double& lastTime, const int fps) {
  currentTime = DisplayManager::getTime();
  delta += currentTime - lastTime;
  lastTime = currentTime;
  if (delta >= 1.0/fps) {
    delta -= 1.0/fps;
    ++TIMER;
    return true;
  } else {
    return false;
  }
}

void updateFPSCount(double& previousSecond, int& updates) {
  if (DisplayManager::getTime() - previousSecond < 1.0) {
    return;
  }

  ++previousSecond;
  cout << "FPS: " << updates << "\n";
  updates = 0;
}

int main() {
  // initialization
  Parser::parse();
  DisplayManager::createDisplay();
  Geometry::initGeometry();
  primaryCamera.initPrimaryCamera();
  Renderer renderer;

  Airplane airplane;
  Sky sky;
  ObstacleHolder obstacles;

  int fps = 60;
  double currentTime, lastTime = DisplayManager::getTime();
  double previousSecond = lastTime;
  double delta = 0;
  int updates = 0;

  while (!DisplayManager::shouldCloseDisplay()) {
    if (shouldUpdate(currentTime, delta, lastTime, fps)) {
      // temporary code for updating game angle
      GAME::AIRPLANE_DISTANCE += GAME::SPEED;

      MouseManager::update();
      primaryCamera.update();
      DisplayManager::prepareDisplay();

      renderer.render();

      obstacles.update();
      airplane.update();
      sky.update();
      DisplayManager::updateDisplay();
      ++updates;
    }

    if (GAME::DISPLAY_FPS)
      updateFPSCount(previousSecond, updates);
  }

  // clean up
  DisplayManager::cleanDisplay();
  Geometry::cleanGeometry();
  return 0;
}
