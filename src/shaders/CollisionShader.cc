#if 0
// Collision.cc
#include "CollisionShader.h"
#include <common.h>
#include <utils/Debug.h>
#include <io/MouseManager.h>
#include <entities/gameObjects/Airplane.h>
#include <entities/gameObjects/Camera.h>
#include <glm/gtc/matrix_transform.hpp>
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
  location_color = getUniformLocation("color");
}


#define C_WIDTH 400
#define C_HEIGHT 300
#define BUFFER_SIZE 4 * C_WIDTH * C_HEIGHT
unsigned char data[BUFFER_SIZE];
int colorId[C_WIDTH * C_HEIGHT];

void CollisionShader::render(const std::vector<DynamicEntity*>& entities) {
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glDisable(GL_BLEND);
  glDisable(GL_MULTISAMPLE);
  // change viewport
  glViewport(0, 0, C_WIDTH, C_HEIGHT); // choose a smaller viewport
  start();
  loadMatrix4f(location_viewMatrix, glm::perspective(Camera::primary().getFov(), 800.0f / 600.0f, NEAR_PLANE, FAR_PLANE));
  loadMatrix4f(location_viewMatrix, Camera::primary().getViewMatrix());
  loadMatrix4f(location_projectionMatrix, Camera::primary().getProjectionMatrix());
  RawModel* model = nullptr;
  RawModel::unbind();
  for (int i = 0; i < entities.size(); ++i) {
    int r = (i & 0x00ff0000) >> 16;
    int g = (i & 0x0000ff00) >> 8;
    int b = (i & 0x000000ff) >> 0;
    loadVector3f(location_color, glm::vec3(r/255.0f, g/255.0f, b/255.0f));

    DynamicEntity* entity = entities[i];
    RawModel* model = entity->getModel();
    model->bind();
    loadMatrix4f(location_transformationMatrix, entity->getTransformationMatrix());

    glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());

    RawModel::unbind();
  }

  loadVector3f(location_color, glm::vec3(1.0f, 0.0f, 0.0f));
  for (int i = 0; i < Airplane::rigidBody.size(); ++i) {
    Entity* entity = Airplane::rigidBody[i];
    RawModel* model = entity->getModel();
    model->bind();
    loadMatrix4f(location_transformationMatrix, entity->getTransformationMatrix());

    glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());

    RawModel::unbind();
  }
  stop();
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_BLEND);
  glFlush();
  glFinish();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glReadPixels(0, 0, C_WIDTH, C_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, data);
  int planeId = 256 * 256 * 256;
  int pixel = 0;
  for (int i = 0; i < C_WIDTH * C_HEIGHT; ++i) {
    colorId[i] = 256 * 256 * data[i * 4] + 256 * data[i * 4 + 1] + data[i * 4 + 2];
    if (colorId[i] == planeId)
      ++pixel;
  }
  std::cout << "pixel " << pixel << "\n";
}

CollisionShader& CollisionShader::theOne() {
  static CollisionShader collisionShader;
  return collisionShader;
}
#endif
