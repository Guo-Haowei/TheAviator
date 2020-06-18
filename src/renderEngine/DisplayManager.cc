// DisplayManager.cc
#include "DisplayManager.h"
#include <GLFW/glfw3.h>
#include <io/KeyboardManager.h>
#include <common.h>
#include <iostream>

using std::cout;

int WIDTH, HEIGHT, ACTUAL_WIDTH, ACTUAL_HEIGHT;

GLFWwindow* DisplayManager::window;

void keyCallback(GLFWwindow* window, int key, int scancodem, int action, int mode);

void DisplayManager::createDisplay() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  WIDTH = (mode->width) * 0.7;
  HEIGHT = (mode->height) * 0.7;

  // window = glfwCreateWindow(WIDTH, HEIGHT, "The Aviator", glfwGetPrimaryMonitor(), NULL);
  window = glfwCreateWindow(WIDTH, HEIGHT, "The Aviator", NULL, NULL);

  if (window == nullptr) {
    cout << "====================================\n";
    cout << "ERROR::GLFW: Failed to create window\n";
    cout << "====================================\n";
  }

  glfwGetFramebufferSize(window, &ACTUAL_WIDTH, &ACTUAL_HEIGHT);
  glfwMakeContextCurrent(window);

  glfwSetKeyCallback(window, keyCallback);
  //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

  if (gladLoadGL() == 0) {
    cout << "======================================\n";
    cout << "ERROR::GLAD: Failed to initialize glad\n";
    cout << "======================================\n";
  }

  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  // alpha blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // face culling
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW);
  glCullFace(GL_BACK);
}

void DisplayManager::prepareDisplay() {
  glfwPollEvents();
}

void DisplayManager::updateDisplay() {
  glfwSwapBuffers(window);
}

void DisplayManager::cleanDisplay() {
  glfwTerminate();
}

bool DisplayManager::shouldCloseDisplay() {
  return glfwWindowShouldClose(window);
}

long double DisplayManager::getTime() {
  return glfwGetTime();
}

void DisplayManager::getCursorPos(double* x, double* y) {
  glfwGetCursorPos(window, x, y);
}

void DisplayManager::setTitle(const char* title) {
  glfwSetWindowTitle(window, title);
}

void keyCallback(GLFWwindow* window, int key, int scancodem, int action, int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
  if (key >= 0 && key < 1024) {
    if (action == GLFW_PRESS)
      KeyboardManager::setKeyDown(key);
    else if (action == GLFW_RELEASE)
      KeyboardManager::setKeyUp(key);
  }
}
