// main.cc
#include "renderEngine/DisplayManager.h"
#include "renderEngine/Renderer.h"
#include <string>
#include <iostream>

bool shouldUpdate(double& currentTime, double& delta, double& lastTime, const int fps) {
  currentTime = DisplayManager::getTime();
  delta += currentTime - lastTime;
  lastTime = currentTime;
  if (delta >= 1.0/fps) {
    delta -= 1.0/fps;
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
  DisplayManager::setTitle(("The Aviator -- FPS: " + std::to_string(updates)).c_str());
  updates = 0;
}

int main() {
  DisplayManager::createDisplay();
  Renderer renderer;

  int fps = 60;
  double currentTime, lastTime = DisplayManager::getTime();
  double previousSecond = lastTime;
  double delta = 0;
  int updates = 0;

  while (!DisplayManager::shouldCloseDisplay()) {
    if (shouldUpdate(currentTime, delta, lastTime, fps)) {
      DisplayManager::prepareDisplay();

      renderer.prepare();

      DisplayManager::updateDisplay();
      ++updates;
    }

    updateFPSCount(previousSecond, updates);
  }

  DisplayManager::cleanDisplay();
  return 0;
}
