// Collision.cc
#include "CollisionShader.h"
#include <common.h>
#include <utils/Debug.h>
#include <entities/gameObjects/Airplane.h>
#include <entities/gameObjects/Camera.h>
#include <GL/glew.h>
#include <iostream>

CollisionShader::CollisionShader() {
  const char* VERTEX_FILE = "../shaders/collision.vert";
  const char* FRAGMENT_FILE = "../shaders/collision.frag";
  ShaderProgram::init(VERTEX_FILE, FRAGMENT_FILE);
}

void CollisionShader::bindAttributes() {
  bindAttribute(0, "position");
}

void CollisionShader::getAllUniformLocations() {
  location_transformationMatrix = getUniformLocation("transformationMatrix");
  location_projectionMatrix = getUniformLocation("projectionMatrix");
  location_viewMatrix = getUniformLocation("viewMatrix");

  // use alpha instead of color just in case anti-aliasing change the color
  location_alpha = getUniformLocation("alpha");
}

void CollisionShader::render(const std::vector<DynamicEntity*>& entities) {
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, ACTUAL_WIDTH, ACTUAL_HEIGHT);
  glDisable(GL_CULL_FACE);
  glDisable(GL_BLEND);
  // change viewport
  start();
  loadMatrix4f(location_viewMatrix, Camera::primary().getViewMatrix());
  loadMatrix4f(location_projectionMatrix, Camera::primary().getProjectionMatrix());
  RawModel* model = nullptr;
  RawModel::unbind();
  for (int i = 0; i < entities.size(); ++i) {
    // it's impossible to have more than 256 objects
    loadFloat(location_alpha, (float)i / 255.0f);

    DynamicEntity* entity = entities[i];
    RawModel* model = entity->getModel();
    model->bind();
    loadMatrix4f(location_transformationMatrix, entity->getTransformationMatrix());

    glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());

    RawModel::unbind();
  }

  loadFloat(location_alpha, 1.0f);
  for (int i = 0; i < Airplane::rigidBody.size(); ++i) {
    Entity* entity = Airplane::rigidBody[i];
    RawModel* model = entity->getModel();
    model->bind();
    Debug::printMatrix(entity->getTransformationMatrix(), "transformation");
    loadMatrix4f(location_transformationMatrix, entity->getTransformationMatrix());

    glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());

    RawModel::unbind();
  }
  stop();
  // change viewport back
  glEnable(GL_CULL_FACE);
  glFlush();
  glFinish();
}

CollisionShader& CollisionShader::theOne() {
  static CollisionShader collisionShader;
  return collisionShader;
}