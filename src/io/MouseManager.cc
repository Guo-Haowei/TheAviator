// MouseManager.cc
#include "MouseManager.h"
#include "../common.h"
#include "../renderEngine/DisplayManager.h"

double MouseManager::x = 0;
double MouseManager::y = 0;
double MouseManager::prevX = 0;
double MouseManager::prevY = 0;
bool MouseManager::initialized = false;

void MouseManager::beforeUpdate() {
  DisplayManager::getCursorPos(&x, &y);
}

void MouseManager::afterUpdate() {
  prevX = x;
  prevY = y;
  initialized = true;
}

double MouseManager::getX() {
  return x;
}

double MouseManager::getY() {
  return y;
}

double MouseManager::getDX() {
  return initialized ? x - prevX : 0;
}

double MouseManager::getDY() {
  return initialized ? y - prevY : 0;
}
