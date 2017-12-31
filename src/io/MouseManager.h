// MouseManager.h
#pragma once

class MouseManager {
private:
  static double x;
  static double y;
public:
  static void update();
  static double getX();
  static double getY();
  static double getRawX();
  static double getRawY();
};
