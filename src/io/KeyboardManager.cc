// KeyboardManager.cc
#include "KeyboardManager.h"

int KeyboardManager::keys[1024] = {0};
set<int> KeyboardManager::currentKeys;
set<int> KeyboardManager::downKeys;
set<int> KeyboardManager::upKeys;

void KeyboardManager::update() {
  upKeys.clear();
  for (int i = 0; i < 1024; ++i)
    if (currentKeys.find(i) != currentKeys.end() && !keys[i])
      upKeys.insert(i);

  downKeys.clear();
  for (int i = 0; i < 1024; ++i)
    if (currentKeys.find(i) == currentKeys.end() && keys[i])
      downKeys.insert(i);

  currentKeys.clear();
  for (int i = 0; i < 1024; ++i)
    if (keys[i])
      currentKeys.insert(i);
}

void KeyboardManager::setKeyDown(int key) {
  keys[key] = 1;
}

void KeyboardManager::setKeyUp(int key) {
  keys[key] = 0;
}

bool KeyboardManager::isKeyUp(int key) {
  return !keys[key];
}

bool KeyboardManager::isKeyDown(int key) {
  return keys[key];
}

bool KeyboardManager::isKeyPressed(int key) {
  return downKeys.find(key) != downKeys.end();
}

bool KeyboardManager::isKeyReleased(int key) {
  return upKeys.find(key) != upKeys.end();
}
