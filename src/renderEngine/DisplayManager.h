// DisplayManager.h
#pragma once
#include "glPrerequisites.h"

struct GLFWwindow;

class DisplayManager {
private:
  static GLFWwindow* window;
public:
  static void createDisplay();
  static void prepareDisplay();
  static void updateDisplay();
  static void cleanDisplay();
  static bool shouldCloseDisplay();

  static long double getTime();
  static void getCursorPos(double* x, double* y);
  static void setTitle(const char* title);
};
