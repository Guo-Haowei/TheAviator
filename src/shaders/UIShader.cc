// UIShader.cc
#include "UIShader.h"
#include <common.h>
#include <models/RawModel.h>
#include <models/Loader.h>
#include <GL/glew.h>
#include <vector>
#include <iostream>
using std::vector;
using std::cout;

UIShader::UIShader(): quad(nullptr) {
  vector<float> vertices;
  vertices.push_back(-1.0f);
  vertices.push_back(1.0f);
  vertices.push_back(1.0f);
  vertices.push_back(1.0f);
  vertices.push_back(1.0f);
  vertices.push_back(-1.0f);

  vertices.push_back(1.0f);
  vertices.push_back(-1.0f);
  vertices.push_back(-1.0f);
  vertices.push_back(-1.0f);
  vertices.push_back(-1.0f);
  vertices.push_back(1.0f);
  quad = Loader::loadToVAO(vertices, 2);

  const char* vertexFile = "../shaders/ui.vert";
  const char* fragFile = "../shaders/ui.frag";
  init(vertexFile, fragFile);
}

UIShader::~UIShader() {
  if (quad)
    delete quad;
}

void UIShader::bindAttributes() {
  bindAttribute(0, "position");
}

void UIShader::getAllUniformLocations() {
  location_health = getUniformLocation("health");
  location_width = getUniformLocation("width");
  location_height = getUniformLocation("height");
}

void UIShader::render() {
  start();
  loadFloat(location_health, GAME::HEALTH);
  loadFloat(location_width, (float)ACTUAL_WIDTH);
  loadFloat(location_height, (float)ACTUAL_HEIGHT);
  quad->bind();
  glDisable(GL_DEPTH_TEST);
  glDrawArrays(GL_TRIANGLES, 0, quad->getVertexCount());
  glEnable(GL_DEPTH_TEST);
  RawModel::unbind(1);
  stop();
}