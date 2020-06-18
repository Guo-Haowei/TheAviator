// UIShader.cc
#include "UIShader.h"
#include "glPrerequisites.h"
#include <common.h>
#include <models/RawModel.h>
#include <models/Loader.h>
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

  const char* VERTEX_FILE = "../shaders/ui.vert";
  const char* FRAG_FILE = "../shaders/ui.frag";
  init(VERTEX_FILE, FRAG_FILE);
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
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  loadFloat(location_health, GAME::HEALTH);
  loadFloat(location_width, (float)ACTUAL_WIDTH);
  loadFloat(location_height, (float)ACTUAL_HEIGHT);
  quad->bind();
  glDrawArrays(GL_TRIANGLES, 0, quad->getVertexCount());
  RawModel::unbind(1);
  stop();
}