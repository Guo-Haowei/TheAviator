// DisplayManager.h
#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#ifdef __linux__
#include <GLFW/glfw3.hpp>
#else
#include <GLFW/glfw3.h>
#endif

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
