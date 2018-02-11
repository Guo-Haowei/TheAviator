// MouseManager.cc
#include "MouseManager.h"
#include <common.h>
#include <renderEngine/DisplayManager.h>

double MouseManager::x = 0;
double MouseManager::y = 0;

void MouseManager::update() {
  DisplayManager::getCursorPos(&x, &y);
  x = -1 + x * 2.0f / (float)WIDTH;
  y = 1 - y * 2.0f / (float)HEIGHT;
}

double MouseManager::getX() {
  return x;
}

double MouseManager::getY() {
  return y;
}
