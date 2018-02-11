// MouseManager.h
#pragma once

class MouseManager {
private:
  static double prevX;
  static double prevY;
  static double x;
  static double y;
  static bool initialized;
public:
  static void beforeUpdate();
  static void afterUpdate();
  static double getX();
  static double getY();
  static double getDX();
  static double getDY();
};
