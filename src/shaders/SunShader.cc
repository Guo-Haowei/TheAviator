// SunShader.cc
#include "SunShader.h"
#include <utils/File.h>
#include <models/RawModel.h>
#include <models/Loader.h>
#include <textures/TextureBuilder.h>
#include <entities/gameObjects/Light.h>
#include <entities/gameObjects/Camera.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <vector>
#include <iostream>
using std::cout;
using std::vector;

#define SCALE 50.0

SunShader::SunShader() {
  vector<float> vertices;
  vertices.push_back(-0.5f);
  vertices.push_back(0.5f);
  vertices.push_back(0.5f);
  vertices.push_back(0.5f);
  vertices.push_back(0.5f);
  vertices.push_back(-0.5f);

  vertices.push_back(0.5f);
  vertices.push_back(-0.5f);
  vertices.push_back(-0.5f);
  vertices.push_back(-0.5f);
  vertices.push_back(-0.5f);
  vertices.push_back(0.5f);
  quad = Loader::loadToVAO(vertices, 2);

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

SunShader::~SunShader() {
  if (quad)
    delete quad;
}

void SunShader::getAllUniformLocations() {
  int location_sunTexture = getUniformLocation("sunTexture");
  int location_PVM = getUniformLocation("PVM");
}

void SunShader::render() {
  start();
  loadInt(location_sunTexture, 0);
  glm::mat4 PVM, S;
  S = glm::scale(S, glm::vec3(SCALE));
  PVM = glm::translate(PVM, Light::theOne().getPosition());
  PVM = Camera::primary().getProjectionMatrix() * Camera::primary().getViewMatrix() * PVM * S;
  loadMatrix4f(location_PVM, PVM);
  m_sunTexture.bindToUint(0);
  quad->bind();
  glDisable(GL_DEPTH_TEST);
  glDrawArrays(GL_TRIANGLES, 0, quad->getVertexCount());
  glEnable(GL_DEPTH_TEST);
  RawModel::unbind(1);
  stop();
}
