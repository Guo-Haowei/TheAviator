// SunShader.cc
#include "SunShader.h"
#include <common.h>
#include <utils/File.h>
#include <models/Geometry.h>
#include <textures/TextureBuilder.h>
#include <entities/gameObjects/Light.h>
#include <entities/gameObjects/Camera.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <iostream>
using std::cout;

SunShader::SunShader() {
  quad = Geometry::quad;
  File textureFile("sun", png);
  TextureBuilder builder(textureFile);
  m_sunTexture = builder.anisotropic()->create();
  const char* VERTEX_FILE = "../shaders/sun.vert";
  const char* FRAGMENT_FILE = "../shaders/sun.frag";
  ShaderProgram::init(VERTEX_FILE, FRAGMENT_FILE);
}

void SunShader::bindAttributes() {
  bindAttribute(0, "position");
}

SunShader::~SunShader() {}

void SunShader::getAllUniformLocations() {
  location_sunTexture = getUniformLocation("sunTexture");
  location_transform = getUniformLocation("transform");
}

void SunShader::render() {
  start();
  loadInt(location_sunTexture, 0);
  glEnable(GL_BLEND);
  glm::vec4 sunScreenPos = Camera::primary().getPVMatrix() * glm::vec4(Light::theOne().getPosition(), 1.0f);
  glm::vec2 sunScreenPos2d;
  sunScreenPos2d.x = sunScreenPos.x / sunScreenPos.w;
  sunScreenPos2d.y = sunScreenPos.y / sunScreenPos.w;
  float scaleX = 0.3f;
  float scaleY = scaleX * (float)ACTUAL_WIDTH / (float)ACTUAL_HEIGHT;
  glm::vec4 transform(sunScreenPos2d, scaleX, scaleY);
  loadVector4f(location_transform, transform);
  m_sunTexture.bindToUint(0);
  quad->bind();
  glDisable(GL_DEPTH_TEST);
  glDrawArrays(GL_TRIANGLES, 0, quad->getVertexCount());
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  RawModel::unbind(1);
  stop();
}
