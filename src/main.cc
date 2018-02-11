// main.cc
#include "common.h"
#include "entities/Camera.h"
#include "entities/Entity.h"
#include "entities/Airplane.h"
#include "entities/Sky.h"
#include "models/Geometry.h"
#include "renderEngine/DisplayManager.h"
#include "renderEngine/Renderer.h"
#include "io/Parser.h"
#include "io/MouseManager.h"
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
using std::cout;
using std::vector;

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

  int fps = 60;
  double currentTime, lastTime = DisplayManager::getTime();
  double previousSecond = lastTime;
  double delta = 0;
  int updates = 0;

  while (!DisplayManager::shouldCloseDisplay()) {
    if (shouldUpdate(currentTime, delta, lastTime, fps)) {
      MouseManager::beforeUpdate();
      primaryCamera.update();
      DisplayManager::prepareDisplay();

      renderer.render();

      airplane.update();
      sky.update();
      DisplayManager::updateDisplay();
      MouseManager::afterUpdate();
      ++updates;
    }

    updateFPSCount(previousSecond, updates);
  }

  // clean up
  DisplayManager::cleanDisplay();
  Geometry::cleanGeometry();
  return 0;
}
