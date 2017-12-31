// MouseManager.cc
#include "MouseManager.h"
#include <common.h>
#include <renderEngine/DisplayManager.h>

double MouseManager::x = 0;
double MouseManager::y = 0;

void MouseManager::update() {
  DisplayManager::getCursorPos(&x, &y);
}

double MouseManager::getX() {
  return -1 + x * 2.0f / (float)WIDTH;
}

double MouseManager::getY() {
  return 1 - y * 2.0f / (float)HEIGHT;
}

double MouseManager::getRawX() {
  return x;
}

double MouseManager::getRawY() {
  return y;
}