// MouseManager.cc
#include "MouseManager.h"
#include "../renderEngine/DisplayManager.h"

double MouseManager::x = 0;
double MouseManager::y = 0;

void MouseManager::update() {
  DisplayManager::getCursorPos(&x, &y);
}

double MouseManager::getX() {
  return x;
}

double MouseManager::getY() {
  return y;
}
