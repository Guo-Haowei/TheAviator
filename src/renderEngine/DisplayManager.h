// DisplayManager.h
#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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
