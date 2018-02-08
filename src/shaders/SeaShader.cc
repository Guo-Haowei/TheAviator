// SeaShader.cc
#include "SeaShader.h"
#include "../common.h"
#include "../models/Geometry.h"
#include "../utils/Debug.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <iostream>
using std::cout;

Entity* SEA_MODEL;

SeaShader::SeaShader() {
  SEA_MODEL = new Entity(Geometry::sea, glm::vec3(0.0f, -SEA::RADIUS - 10.0f, 0.0f), glm::vec3(90.0f, 0.0f, 0.0f));

  const char* VERTEX_FILE = "../shaders/sea.vert";
  const char* FRAGMENT_FILE = "../shaders/sea.frag";
  const char* GEOMETRY_FILE = "../shaders/sea.geom";
  init(VERTEX_FILE, FRAGMENT_FILE, GEOMETRY_FILE);
}

void SeaShader::clean() {
  stop();
  glDetachShader(programID, vertexShaderID);
  glDeleteShader(vertexShaderID);
  if (geometryShaderID != 0) {
    glDetachShader(programID, geometryShaderID);
    glDeleteShader(geometryShaderID);
  }
  glDetachShader(programID, fragmentShaderID);
  glDeleteShader(fragmentShaderID);
  glDeleteProgram(programID);
  delete SEA_MODEL;
}

void SeaShader::bindAttributes() {
  bindAttribute(0, "position");
  bindAttribute(1, "wave");
}

void SeaShader::getAllUniformLocations() {
  ShaderProgram::getAllUniformLocations();
  location_projectionMatrix = getUniformLocation("projectionMatrix");
  location_viewMatrix = getUniformLocation("viewMatrix");
  location_time = getUniformLocation("time");
  location_light = getUniformLocation("lightPos");
  location_shadowMap = getUniformLocation("shadowMap");
}

void SeaShader::render() {
  start();
  glm::vec3 lightPos(LIGHT::X, LIGHT::Y, LIGHT::Z);
  glm::mat4 viewMatrix = glm::lookAt(lightPos, glm::vec3(AIRPLANE::X, AIRPLANE::Y, AIRPLANE::Z), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera->getZoom()), (float) ACTUAL_WIDTH / (float) ACTUAL_HEIGHT, SHADOW::NEAR_PLANE, SHADOW::FAR_PLANE);
  loadMatrix4f(location_lightSpaceMatrix, projectionMatrix * viewMatrix);
  loadInt(location_shadowMap, 0);
  loadVector3f(location_light, lightPos);
  loadFloat(location_time, TIMER);
  loadMatrix4f(location_viewMatrix, camera->getViewMatrix());
  projectionMatrix = glm::perspective(glm::radians(camera->getZoom()), (float) ACTUAL_WIDTH / (float) ACTUAL_HEIGHT, NEAR_PLANE, FAR_PLANE);
  loadMatrix4f(location_projectionMatrix, projectionMatrix);
  RawModel* model = SEA_MODEL->getModel();
  loadMatrix4f(location_transformationMatrix, SEA_MODEL->getTransformationMatrix());
  model->bind();

  glEnable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  glDrawElements(GL_TRIANGLES, model->getVertexCount(), GL_UNSIGNED_INT, (void*) 0);
  glDisable(GL_BLEND);
  glDisable(GL_CULL_FACE);

  RawModel::unbind();
  stop();

  // update sea
  SEA_MODEL->changeRotation(0.0f, 0.1f, 0.0f); // move this line to other place later
}

SeaShader* SeaShader::setCamera(Camera* camera) {
  this->camera = camera;
  return this;
}
