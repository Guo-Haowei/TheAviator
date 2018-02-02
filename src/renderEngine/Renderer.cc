// Renderer.cc
#include "Renderer.h"
#include "../common.h"
#include "../entities/Entity.h"
#include "../entities/Camera.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

Renderer::Renderer(): seaShadowShader(true) {
  entityShader.setCamera(&primaryCamera)->setEntities(&allEntities);
  seaShader.setCamera(&primaryCamera);
  seaShadowShader.setCamera(&primaryCamera)->setEntities(&allEntities);
}

Renderer::~Renderer() {
  backgroundShader.clean();
  entityShader.clean();
  seaShader.clean();
}

void Renderer::render() {
  // render to depth map
  glViewport(0, 0, ACTUAL_WIDTH, ACTUAL_HEIGHT); // temporary
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  seaShadowShader.render();
  // render the actual scene
  glViewport(0, 0, ACTUAL_WIDTH, ACTUAL_HEIGHT);
  // backgroundShader.render();
  // entityShader.render();
  // seaShader.render();
}
