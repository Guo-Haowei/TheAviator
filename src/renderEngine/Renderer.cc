// Renderer.cc
#define GLEW_STATIC
#include "Renderer.h"
#include <common.h>
#include <entities/Entity.h>
#include <GL/glew.h>
#ifdef __linux__
#include <GLFW/glfw3.hpp>
#else
#include <GLFW/glfw3.h>
#endif
#include <iostream>
using std::cout;

Renderer::Renderer(): seaShadowShader(true) {
  ShadowShader::init();
  // generate gBuffer
  glGenFramebuffers(1, &gBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
  // velocity color buffer
  glGenTextures(1, &gVelocity);
  glBindTexture(GL_TEXTURE_2D, gVelocity);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, ACTUAL_WIDTH, ACTUAL_HEIGHT, 0, GL_RG, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gVelocity, 0);
  // color
  glGenTextures(1, &gColor);
  glBindTexture(GL_TEXTURE_2D, gColor);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ACTUAL_WIDTH, ACTUAL_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gColor, 0);
  // depth
  glGenRenderbuffers(1, &gDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, gDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, ACTUAL_WIDTH, ACTUAL_HEIGHT);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gDepth);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

Renderer::~Renderer() {}

void Renderer::render() {
  // render to veloctiy shader
  // glViewport(0, 0, ACTUAL_WIDTH, ACTUAL_HEIGHT);
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // velocityShader.render();
  // render to depth map
  glViewport(0, 0, SHADOW::WIDTH, SHADOW::HEIGHT); // temporary
  glBindFramebuffer(GL_FRAMEBUFFER, ShadowShader::getFboID());
  glClear(GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_FRONT);
  seaShadowShader.render();
  entityShadowShader.render();
  glCullFace(GL_BACK);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // render the actual scene to image
  glViewport(0, 0, ACTUAL_WIDTH, ACTUAL_HEIGHT);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, ShadowShader::getDepthMap().getID());
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glDrawBuffer(GL_COLOR_ATTACHMENT1);
  backgroundShader.render();
  entityShader.render();
  seaShader.render();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, gColor);
  motionBlurShader.render();
  // render sun and lens flare
  flareShader.render();
  sunShader.render();
  uiShader.render();
  // velocityShader.updateEntityVelocity();
}
