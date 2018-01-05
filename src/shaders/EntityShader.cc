// EntityShader.cc
#include "EntityShader.h"
#include "../common.h"
#include "../utils/Debug.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <iostream>
using std::cout;

EntityShader::EntityShader() {
  const char* VERTEX_FILE = "../shaders/entity.vert";
  const char* FRAGMENT_FILE = "../shaders/entity.frag";
  init(VERTEX_FILE, FRAGMENT_FILE);
}

void EntityShader::bindAttributes() {
  bindAttribute(0, "position");
  bindAttribute(1, "normal");
}

void EntityShader::getAllUniformLocations() {
  location_transformationMatrix = getUniformLocation("transformationMatrix");
  location_projectionMatrix = getUniformLocation("projectionMatrix");
  location_viewMatrix = getUniformLocation("viewMatrix");
  location_color = getUniformLocation("color");
}

void EntityShader::render() {
  start();
  loadMatrix4f(location_viewMatrix, camera->getViewMatrix());
  glm::mat4 projectionMatrix = glm::perspective(camera->getZoom(), (float) ACTUAL_WIDTH / (float) ACTUAL_HEIGHT, NEAR_PLANE, FAR_PLANE);
  loadMatrix4f(location_projectionMatrix, projectionMatrix);
  for (int i = 0; i < entities->size(); ++i) {
    Entity* entity = entities->at(i);
    RawModel* model = entity->getModel();
    loadVector3f(location_color, entity->getColor());
    loadMatrix4f(location_transformationMatrix, entity->getTransformationMatrix());
    model->bind();
    glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());
    RawModel::unbind();
  }
  stop();
}

EntityShader* EntityShader::setCamera(Camera* camera) {
  this->camera = camera;
  return this;
}

EntityShader* EntityShader::setEntities(vector<Entity*>* entities) {
  this->entities = entities;
  return this;
}
