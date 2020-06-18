// BackgroundShader.cc
#include "BackgroundShader.h"
#include "ShaderProgram.h"
#include <common.h>
#include <models/RawModel.h>
#include <models/Loader.h>
#include "glPrerequisites.h"
#include <vector>
using std::vector;

BackgroundShader::BackgroundShader():background(nullptr) {
  vector<float> vertices, colors;

  // first triangle
  vertices.push_back(-1.0f);
  vertices.push_back(1.0f);
  colors.push_back(BACKGROUND_COLOR1[0]);
  colors.push_back(BACKGROUND_COLOR1[1]);
  colors.push_back(BACKGROUND_COLOR1[2]);
  vertices.push_back(1.0f);
  vertices.push_back(1.0f);
  colors.push_back(BACKGROUND_COLOR1[0]);
  colors.push_back(BACKGROUND_COLOR1[1]);
  colors.push_back(BACKGROUND_COLOR1[2]);
  vertices.push_back(1.0f);
  vertices.push_back(-1.0f);
  colors.push_back(BACKGROUND_COLOR2[0]);
  colors.push_back(BACKGROUND_COLOR2[1]);
  colors.push_back(BACKGROUND_COLOR2[2]);

  // second triangle
  vertices.push_back(1.0f);
  vertices.push_back(-1.0f);
  colors.push_back(BACKGROUND_COLOR2[0]);
  colors.push_back(BACKGROUND_COLOR2[1]);
  colors.push_back(BACKGROUND_COLOR2[2]);
  vertices.push_back(-1.0f);
  vertices.push_back(-1.0f);
  colors.push_back(BACKGROUND_COLOR2[0]);
  colors.push_back(BACKGROUND_COLOR2[1]);
  colors.push_back(BACKGROUND_COLOR2[2]);
  vertices.push_back(-1.0f);
  vertices.push_back(1.0f);
  colors.push_back(BACKGROUND_COLOR1[0]);
  colors.push_back(BACKGROUND_COLOR1[1]);
  colors.push_back(BACKGROUND_COLOR1[2]);

  background = Loader::loadToVAO(vertices, 2, colors, 3);

  const char* vertexFile = "../shaders/background.vert";
  const char* fragFile = "../shaders/background.frag";
  init(vertexFile, fragFile);
}

BackgroundShader::~BackgroundShader() {
  if (background)
    delete background;
}

void BackgroundShader::bindAttributes() {
  bindAttribute(0, "position");
  bindAttribute(1, "color");
}

void BackgroundShader::getAllUniformLocations() { }

void BackgroundShader::render() {
  start();
  glEnable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  background->bind();
  glDrawArrays(GL_TRIANGLES, 0, background->getVertexCount());
  RawModel::unbind();
  stop();
}
