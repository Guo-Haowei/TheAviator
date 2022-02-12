// Renderer.cc
#include "Renderer.h"
#include "glPrerequisites.h"
#include <GLFW/glfw3.h>
#include <common.h>
#include <entities/Entity.h>
#include <iostream>

using std::cout;

Renderer::Renderer() : seaShadowShader(true) {
  ShadowShader::init();
  // depth
  glGenRenderbuffers(1, &depthBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, ACTUAL_WIDTH,
                        ACTUAL_HEIGHT);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, depthBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

Renderer::~Renderer() {}

void Renderer::render() {
  // render to depth map
  glViewport(0, 0, SHADOW::WIDTH, SHADOW::HEIGHT); // temporary
  glBindFramebuffer(GL_FRAMEBUFFER, ShadowShader::getFboID());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_FRONT);
  seaShadowShader.render();
  entityShadowShader.render();
  glCullFace(GL_BACK);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glViewport(0, 0, ACTUAL_WIDTH, ACTUAL_HEIGHT);
  // render the actual scene to image
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, ShadowShader::getDepthMap().getID());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  backgroundShader.render();
  entityShader.render();
  seaShader.render();

  // render ui
  uiShader.render();
}
