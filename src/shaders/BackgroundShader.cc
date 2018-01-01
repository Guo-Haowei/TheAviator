// BackgroundShader.cc
#include "BackgroundShader.h"
#include "ShaderProgram.h"
#include "../common.h"
#include "../models/Loader.h"
#include <vector>
using std::vector;

BackgroundShader::BackgroundShader() {
  vector<float> vertices, normals, colors;

  // first triangle
  // vertices.push_back(-1.0f);
  // vertices.push_back(1.0f);
  // vertices.push_back(0.0f);
  // colors.push_back(BACKGROUND_COLOR1[0]);
  // colors.push_back(BACKGROUND_COLOR1[1]);
  // colors.push_back(BACKGROUND_COLOR1[2]);
  // vertices.push_back(1.0f);
  // vertices.push_back(1.0f);
  // vertices.push_back(0.0f);
  // colors.push_back(BACKGROUND_COLOR1[0]);
  // colors.push_back(BACKGROUND_COLOR1[1]);
  // colors.push_back(BACKGROUND_COLOR1[2]);
  // vertices.push_back(1.0f);
  // vertices.push_back(-1.0f);
  // vertices.push_back(0.0f);
  // colors.push_back(BACKGROUND_COLOR2[0]);
  // colors.push_back(BACKGROUND_COLOR2[1]);
  // colors.push_back(BACKGROUND_COLOR2[2]);

  vertices.push_back(1.0f);
  vertices.push_back(-1.0f);
  vertices.push_back(0.0f);
  colors.push_back(BACKGROUND_COLOR2[0]);
  colors.push_back(BACKGROUND_COLOR2[1]);
  colors.push_back(BACKGROUND_COLOR2[2]);
  vertices.push_back(-1.0f);
  vertices.push_back(-1.0f);
  vertices.push_back(0.0f);
  colors.push_back(BACKGROUND_COLOR1[0]);
  colors.push_back(BACKGROUND_COLOR1[1]);
  colors.push_back(BACKGROUND_COLOR1[2]);
  vertices.push_back(-1.0f);
  vertices.push_back(1.0f);
  vertices.push_back(0.0f);
  colors.push_back(BACKGROUND_COLOR1[0]);
  colors.push_back(BACKGROUND_COLOR1[1]);
  colors.push_back(BACKGROUND_COLOR1[2]);

  // second triangle
  // vertices.push_back(1.0f);
  // vertices.push_back(-1.0f);
  // vertices.push_back(0.0f);
  // colors.push_back(BACKGROUND_COLOR1[0]);
  // colors.push_back(BACKGROUND_COLOR1[1]);
  // colors.push_back(BACKGROUND_COLOR1[2]);
  // vertices.push_back(-1.0f);
  // vertices.push_back(-1.0f);
  // vertices.push_back(0.0f);
  // colors.push_back(BACKGROUND_COLOR2[0]);
  // colors.push_back(BACKGROUND_COLOR2[1]);
  // colors.push_back(BACKGROUND_COLOR2[2]);
  // vertices.push_back(-1.0f);
  // vertices.push_back(1.0f);
  // vertices.push_back(0.0f);
  // colors.push_back(BACKGROUND_COLOR2[0]);
  // colors.push_back(BACKGROUND_COLOR2[1]);
  // colors.push_back(BACKGROUND_COLOR2[2]);

  background = Loader::loadToVAO(vertices, normals, colors);

  const char* vertexFile = "../shaders/background.vert";
  const char* fragFile = "../shaders/background.frag";
  init(vertexFile, fragFile);
}

BackgroundShader::~BackgroundShader() {
  delete background;
}

RawModel* BackgroundShader::getModel() {
  return background;
}

void BackgroundShader::bindAttributes() {
  bindAttribute(0, "position");
  bindAttribute(1, "color");
}

void BackgroundShader::getAllUniformLocations() { }
