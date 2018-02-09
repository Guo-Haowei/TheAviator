// Renderer.cc
#define GLEW_STATIC
#include "Renderer.h"
#include "../common.h"
#include "../entities/Entity.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

Renderer::Renderer(): seaShadowShader(true) {
  ShadowShader::init();
}

Renderer::~Renderer() {
  backgroundShader.clean();
  entityShader.clean();
  entityShadowShader.clean();
  seaShader.clean();
  seaShadowShader.clean();

}

void Renderer::render() {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  // render to depth map
  glViewport(0, 0, SHADOW::WIDTH, SHADOW::HEIGHT); // temporary
  glBindFramebuffer(GL_FRAMEBUFFER, ShadowShader::getFboID());
  glClear(GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_FRONT);
  seaShadowShader.render();
  entityShadowShader.render();
  glCullFace(GL_BACK);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // render the actual scene
  glViewport(0, 0, ACTUAL_WIDTH, ACTUAL_HEIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, ShadowShader::getDepthMap());
#if DB_DEPTHMAP
  dbDepthMapSahder.render();
#else
  backgroundShader.render();
  entityShader.render();
  seaShader.render();
#endif
}
