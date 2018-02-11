// EntityShader.cc
#include "EntityShader.h"
#include "../common.h"
#include "../utils/Debug.h"
#include "../entities/Entity.h"
#include <GL/glew.h>
#include <iostream>
using std::cout;
using std::vector;

EntityShader::EntityShader() {
  const char* VERTEX_FILE = "../shaders/entity.vert";
  const char* FRAGMENT_FILE = "../shaders/entity.frag";
  ShaderProgram::init(VERTEX_FILE, FRAGMENT_FILE);
  camera = &primaryCamera;
}

void EntityShader::bindAttributes() {
  bindAttribute(0, "position");
  bindAttribute(1, "normal");
}

void EntityShader::getAllUniformLocations() {
  ShaderProgram::getAllUniformLocations();
  location_projectionMatrix = getUniformLocation("projectionMatrix");
  location_viewMatrix = getUniformLocation("viewMatrix");
  location_color = getUniformLocation("color");
  location_light = getUniformLocation("lightPos");
  location_shadowMap = getUniformLocation("shadowMap");
  location_opacity = getUniformLocation("opacity");
  location_receiveShadow = getUniformLocation("receiveShadow");
}

void EntityShader::render() {
  start();
  glm::vec3 lightPos(LIGHT::X, LIGHT::Y, LIGHT::Z);
  loadInt(location_shadowMap, 0);
  loadVector3f(location_light, lightPos);
  loadMatrix4f(location_lightSpaceMatrix, camera->getLightSpaceMatrix());
  loadMatrix4f(location_viewMatrix, camera->getViewMatrix());
  loadMatrix4f(location_projectionMatrix, camera->getProjectionMatrix());
  for (auto& entry: allEntities) {
    vector<Entity*>* entities = &(entry.second);
    entry.first->bind();
    for (int i = 0; i < entities->size(); ++i) {
      Entity* entity = entities->at(i);
      RawModel* model = entity->getModel();
      loadBool(location_receiveShadow, entity->getReceiveShadow());
      loadFloat(location_opacity, entity->getOpacity());
      loadVector3f(location_color, entity->getColor());
      loadMatrix4f(location_transformationMatrix, entity->getTransformationMatrix());
      if (entity->getOpacity() < 1.0f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);
        glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());
        glDisable(GL_BLEND);
      } else {
        glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());
      }

    }
    RawModel::unbind();
  }
  stop();
}
