// KeyboardManager.h
#pragma once
#include <set>
using std::set;

class KeyboardManager {
private:
  static int keys[1024];
  static set<int> currentKeys;
  static set<int> downKeys;
  static set<int> upKeys;
public:
  static void update();
  static void setKeyDown(int key);
  static void setKeyUp(int key);
  static bool isKeyUp(int key);
  static bool isKeyDown(int key);
  static bool isKeyPressed(int key);
  static bool isKeyReleased(int key);
};

enum KEYS {
  KEY_0 = 48,
  KEY_1 = 49,
  KEY_2 = 50,
  KEY_3 = 51,
  KEY_4 = 52,
  KEY_5 = 53,
  KEY_6 = 54,
  KEY_7 = 55,
  KEY_8 = 56,
  KEY_9 = 57,
  KEY_A = 65,
  KEY_B = 66,
  KEY_C = 67,
  KEY_D = 68,
  KEY_E = 69,
  KEY_F = 70,
  KEY_G = 71,
  KEY_H = 72,
  KEY_I = 73,
  KEY_J = 74,
  KEY_K = 75,
  KEY_L = 76,
  KEY_M = 77,
  KEY_N = 78,
  KEY_O = 79,
  KEY_P = 80,
  KEY_Q = 81,
  KEY_R = 82,
  KEY_S = 83,
  KEY_T = 84,
  KEY_U = 85,
  KEY_V = 86,
  KEY_W = 87,
  KEY_X = 88,
  KEY_Y = 89,
  KEY_Z = 90,
  KEY_RIGHT = 262,
  KEY_LEFT = 263,
  KEY_DOWN = 264,
  KEY_UP = 265,

  KEY_TAB = 258,
  KEY_LSB = 91,
  KEY_RSB = 93,
  KEY_DEL = 261,
  KEY_BACKSPACE = 259
};
