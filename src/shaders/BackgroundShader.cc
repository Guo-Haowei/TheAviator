// BackgroundShader.cc
#include "BackgroundShader.h"
#include "ShaderProgram.h"
#include "../common.h"
#include "../models/Loader.h"
#include <GL/glew.h>
#include <vector>
using std::vector;

BackgroundShader::BackgroundShader() {
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
  delete background;
}

void BackgroundShader::bindAttributes() {
  bindAttribute(0, "position");
  bindAttribute(1, "color");
}

void BackgroundShader::getAllUniformLocations() { }

void BackgroundShader::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  start();
  background->bind();
  glDisable(GL_DEPTH_TEST);
  glDrawArrays(GL_TRIANGLES, 0, background->getVertexCount());
  glEnable(GL_DEPTH_TEST);
  RawModel::unbind();
  stop();
}
