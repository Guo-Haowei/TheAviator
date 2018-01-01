// DisplayManager.cc
#include "DisplayManager.h"
#include "../common.h"
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

  const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  WIDTH = mode->width;
  HEIGHT = mode->height;

  window = glfwCreateWindow(WIDTH, HEIGHT, "The Aviator", glfwGetPrimaryMonitor(), NULL);

  if (window == nullptr) {
    cout << "====================================\n";
    cout << "ERROR::GLFW: Failed to create window\n";
    cout << "====================================\n";
  }

  glfwGetFramebufferSize(window, &ACTUAL_WIDTH, &ACTUAL_HEIGHT);
  glfwMakeContextCurrent(window);

  // callbacks
  glfwSetKeyCallback(window, keyCallback);

  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    cout << "======================================\n";
    cout << "ERROR::GLEW: Failed to initialize GLEW\n";
    cout << "======================================\n";
  }

  // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glViewport(0, 0, ACTUAL_WIDTH, ACTUAL_HEIGHT);
  glEnable(GL_DEPTH_TEST);
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
}
