// Renderer.cc
#include "Renderer.h"
#include "glPrerequisites.h"
#include <common.h>
#include <entities/Entity.h>
#include <GLFW/glfw3.h>
#include <iostream>
using std::cout;

Renderer::Renderer(): seaShadowShader(true) {
  ShadowShader::init();
  // generate buffer
  glGenFramebuffers(1, &frameBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
  // color
  glGenTextures(1, &colorTexture);
  glBindTexture(GL_TEXTURE_2D, colorTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ACTUAL_WIDTH, ACTUAL_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
  // velocity
  glGenTextures(1, &velocityTexture);
  glBindTexture(GL_TEXTURE_2D, velocityTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, ACTUAL_WIDTH, ACTUAL_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, velocityTexture, 0);
  // draw to color attachment
  unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
  glDrawBuffers(2, attachments);

  // depth
  glGenRenderbuffers(1, &depthBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, ACTUAL_WIDTH, ACTUAL_HEIGHT);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
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
  glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, ShadowShader::getDepthMap().getID());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  backgroundShader.render();
  entityShader.render();
  seaShader.render();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // motion blur
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, colorTexture);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, velocityTexture);
  motionBlurShader.render();

  // render ui
  uiShader.render();

  entityShader.updateEntityVelocity();
}
