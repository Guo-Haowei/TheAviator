// VelocityShader.cc
#include "VelocityShader.h"
#include <entities/Entity.h>
#include <entities/DynamicEntity.h>
#include <entities/gameObjects/Camera.h>
#include <entities/gameObjects/ParticleHolder.h>
#include <utils/Debug.h>
#include <GL/glew.h>
#include <iostream>
using std::cout;
using std::vector;

VelocityShader::VelocityShader() {
  const char* VERTEX_FILE = "../shaders/velocity.vert";
  const char* FRAGMENT_FILE = "../shaders/velocity.frag";
  ShaderProgram::init(VERTEX_FILE, FRAGMENT_FILE);
}

VelocityShader::~VelocityShader() {}

void VelocityShader::bindAttributes() {
  bindAttribute(0, "position");
}

void VelocityShader::getAllUniformLocations() {
  location_PVM = getUniformLocation("PVM");
  location_prevPVM = getUniformLocation("prevPVM");
}

void VelocityShader::render() {
  start();
  glDisable(GL_CULL_FACE);
  glm::mat4 PV = Camera::primary().getPVMatrix();

  for (auto& entry: staticEntities) {
    vector<Entity*>& entities = entry.second;
    entry.first->bind();
    for (int i = 0; i < entities.size(); ++i) {
      Entity* entity = entities.at(i);
      RawModel* model = entity->getModel();
      glm::mat4 prevPVM = PV * entity->getPrevTransformationMatrix();
      glm::mat4 PVM = PV * entity->getTransformationMatrix();
      loadMatrix4f(location_prevPVM, prevPVM);
      loadMatrix4f(location_PVM, PVM);

      glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());
    }
    RawModel::unbind();
  }
  for (auto& entry: dynamicEntities) {
    vector<DynamicEntity*>& entities = entry.second;
    entry.first->bind();
    for (int i = 0; i < entities.size(); ++i) {
      DynamicEntity* entity = entities.at(i);
      RawModel* model = entity->getModel();
      glm::mat4 prevPVM = PV * entity->getPrevTransformationMatrix();
      glm::mat4 PVM = PV * entity->getTransformationMatrix();
      loadMatrix4f(location_prevPVM, prevPVM);
      loadMatrix4f(location_PVM, PVM);

      glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());
    }
    RawModel::unbind();
  }

  vector<DynamicEntity*>& particles = ParticleHolder::getParticles();
  if (particles.size())
    particles[0]->getModel()->bind();
  for (int i = 0; i < particles.size(); ++i) {
    DynamicEntity* particle = particles.at(i);
    RawModel* model = particle->getModel();
    glm::mat4 prevPVM = PV * particle->getPrevTransformationMatrix();
    glm::mat4 PVM = PV * particle->getTransformationMatrix();
    loadMatrix4f(location_prevPVM, prevPVM);
    loadMatrix4f(location_PVM, PVM);

    glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());
  }
  RawModel::unbind();
  glEnable(GL_CULL_FACE);
  stop();
}

void VelocityShader::updateEntityVelocity() {
  for (auto& entry: staticEntities) {
    vector<Entity*>& entities = entry.second;
    for (int i = 0; i < entities.size(); ++i) {
      entities.at(i)->updatePrevTransformation();
    }
  }
  for (auto& entry: dynamicEntities) {
    vector<DynamicEntity*>& entities = entry.second;
    for (int i = 0; i < entities.size(); ++i) {
      entities.at(i)->updatePrevTransformation();
    }
  }

  vector<DynamicEntity*>& particles = ParticleHolder::getParticles();
  for (int i = 0; i < particles.size(); ++i) {
    particles.at(i)->updatePrevTransformation();
  }
}
