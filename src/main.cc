// main.cc
#include "common.h"
#include "entities/Camera.h"
#include "entities/Entity.h"
#include "entities/AirPlane.h"
#include "models/Geometry.h"
#include "renderEngine/DisplayManager.h"
#include "renderEngine/Renderer.h"
#include "utils/Parser.h"
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
  Renderer renderer;
  primaryCamera.initPrimaryCamera();
  // Entity entity1(Geometry::cockpit, glm::vec3(0.0f, 100.0f, 150.0f), glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f));
  // Entity entity2(Geometry::cube, glm::vec3(10.0f, 110.0f, 175.0f), glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(20.f, 1.0f, 20.0f));
  // allEntities.push_back(&entity1);
  // allEntities.push_back(&entity2);

  AirPlane airPlane;

  int fps = 60;
  double currentTime, lastTime = DisplayManager::getTime();
  double previousSecond = lastTime;
  double delta = 0;
  int updates = 0;

  while (!DisplayManager::shouldCloseDisplay()) {
    if (shouldUpdate(currentTime, delta, lastTime, fps)) {
      DisplayManager::prepareDisplay();

      renderer.render();

      // entity1.changeRotation(0.0f, 1.0f, 0.0f);
      DisplayManager::updateDisplay();
      ++updates;
    }

    updateFPSCount(previousSecond, updates);
  }

  // clean up
  DisplayManager::cleanDisplay();
  Geometry::cleanGeometry();
  return 0;
}
