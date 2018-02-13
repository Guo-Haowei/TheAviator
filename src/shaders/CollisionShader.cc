// Collision.cc
#include "CollisionShader.h"
#include <common.h>
#include <utils/Debug.h>
#include <entities/gameObjects/Airplane.h>
#include <GL/glew.h>

CollisionShader::CollisionShader() {
  const char* VERTEX_FILE = "../shaders/collision.vert";
  const char* FRAGMENT_FILE = "../shaders/collision.frag";
  ShaderProgram::init(VERTEX_FILE, FRAGMENT_FILE);
  camera = &primaryCamera;
}

void CollisionShader::bindAttributes() {
  bindAttribute(0, "position");
}

void CollisionShader::getAllUniformLocations() {
  location_transformationMatrix = getUniformLocation("transformationMatrix");
  location_projectionMatrix = getUniformLocation("projectionMatrix");
  location_viewMatrix = getUniformLocation("viewMatrix");
  location_color = getUniformLocation("color");
}

void CollisionShader::render(const std::vector<DynamicEntity*>& entities) {
  // change viewport
  start();
  loadMatrix4f(location_viewMatrix, camera->getViewMatrix());
  loadMatrix4f(location_projectionMatrix, camera->getProjectionMatrix());
  RawModel* model = nullptr;
  RawModel::unbind();
  for (int i = 0; i < entities.size(); ++i) {
    int r = (i & 0x000000ff) >> 0;
    int g = (i & 0x0000ff00) >> 8;
    int b = (i & 0x00ff0000) >> 16;
    loadVector3f(location_color, glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f));

    DynamicEntity* entity = entities[i];
    if (model != entity->getModel()) {
      model = entity->getModel();
      RawModel::unbind();
      model->bind();
    }

    loadMatrix4f(location_transformationMatrix, entity->getTransformationMatrix());
    glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());
  }

  RawModel::unbind();
  model = nullptr;
  loadVector3f(location_color, glm::vec3(1.0f, 0.0f, 0.0f));
  for (int i = 0; i < Airplane::rigidBody.size(); ++i) {
    Entity* entity = Airplane::rigidBody[i];
    if (model != entity->getModel()) {
      model = entity->getModel();
      RawModel::unbind();
      model->bind();
    }

    loadMatrix4f(location_transformationMatrix, entity->getTransformationMatrix());
    glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());
  }
  stop();
  // change viewport back
}

CollisionShader& CollisionShader::theOne() {
  CollisionShader collisionShader;
  return collisionShader;
}