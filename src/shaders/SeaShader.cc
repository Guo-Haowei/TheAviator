// SeaShader.cc
#include "SeaShader.h"
#include "../common.h"
#include "../models/Geometry.h"
#include "../utils/Debug.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <iostream>
using std::cout;

SeaShader::SeaShader() {
  sea = new Entity(Geometry::sea, glm::vec3(0.0f, -150.0f, -280.0f), glm::vec3(90.0f, 0.0f, 0.0f));

  const char* VERTEX_FILE = "../shaders/sea.vert";
  const char* FRAGMENT_FILE = "../shaders/sea.frag";
  init(VERTEX_FILE, FRAGMENT_FILE);
}

void SeaShader::clean() {
  stop();
  glDetachShader(programID, vertexShaderID);
  glDetachShader(programID, fragmentShaderID);
  glDeleteShader(vertexShaderID);
  glDeleteShader(fragmentShaderID);
  glDeleteProgram(programID);
  delete sea;
}

void SeaShader::bindAttributes() {
  bindAttribute(0, "position");
  bindAttribute(1, "wave");
}

void SeaShader::getAllUniformLocations() {
  location_transformationMatrix = getUniformLocation("transformationMatrix");
  location_projectionMatrix = getUniformLocation("projectionMatrix");
  location_viewMatrix = getUniformLocation("viewMatrix");
  location_time = getUniformLocation("time");
}

void SeaShader::render() {
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  start();
  loadFloat(location_time, TIMER);
  loadMatrix4f(location_viewMatrix, camera->getViewMatrix());
  glm::mat4 projectionMatrix = glm::perspective(camera->getZoom(), (float) ACTUAL_WIDTH / (float) ACTUAL_HEIGHT, NEAR_PLANE, FAR_PLANE);
  loadMatrix4f(location_projectionMatrix, projectionMatrix);
  RawModel* model = sea->getModel();
  loadMatrix4f(location_transformationMatrix, sea->getTransformationMatrix());
  model->bind();
  glDrawElements(GL_TRIANGLES, model->getVertexCount(), GL_UNSIGNED_INT, (void*) 0);
  RawModel::unbind();
  stop();

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

SeaShader* SeaShader::setCamera(Camera* camera) {
  this->camera = camera;
  return this;
}
