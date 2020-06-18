// MotionBlurShader.cc
#include "MotionBlurShader.h"
#include <models/Geometry.h>
#include "glPrerequisites.h"

MotionBlurShader::MotionBlurShader() {
  quad = Geometry::quad;
  const char* VERTEX_FILE = "../shaders/motionBlur.vert";
  const char* FRAGMENT_FILE = "../shaders/motionBlur.frag";
  ShaderProgram::init(VERTEX_FILE, FRAGMENT_FILE);
}

MotionBlurShader::~MotionBlurShader() {}

void MotionBlurShader::bindAttributes() {
  bindAttribute(0, "position");
}

void MotionBlurShader::getAllUniformLocations() {
  location_colorTexture = getUniformLocation("colorTexture");
  location_velocityTexture = getUniformLocation("velocityTexture");
}

void MotionBlurShader::render() {
  start();
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  loadInt(location_colorTexture, 0);
  loadInt(location_velocityTexture, 1);
  quad->bind();
  glDrawArrays(GL_TRIANGLES, 0, quad->getVertexCount());
  RawModel::unbind();
  stop();
}
