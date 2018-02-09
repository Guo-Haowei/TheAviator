// ShadowShader.cc
#define GLEW_STATIC
#include "ShadowShader.h"
#include "../common.h"
#include "../entities/Entity.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cassert>
#include <iostream>
using std::vector;

unsigned int ShadowShader::fboID;
unsigned int ShadowShader::depthMap;

ShadowShader::ShadowShader(bool isSeaShadow): isSeaShadow(isSeaShadow) {
  if (isSeaShadow) {
    const char* VERTEX_FILE = "../shaders/seaShadow.vert";
    const char* FRAGMENT_FILE = "../shaders/seaShadow.frag";
    ShaderProgram::init(VERTEX_FILE, FRAGMENT_FILE);
  } else {
    const char* VERTEX_FILE = "../shaders/entityShadow.vert";
    const char* FRAGMENT_FILE = "../shaders/entityShadow.frag";
    ShaderProgram::init(VERTEX_FILE, FRAGMENT_FILE);
  }
}

void ShadowShader::init() {
  glGenFramebuffers(1, &fboID);
  glGenTextures(1, &depthMap);

  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW::WIDTH, SHADOW::HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  glBindFramebuffer(GL_FRAMEBUFFER, fboID);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

void ShadowShader::bindAttributes() {
  bindAttribute(0, "position");
  if (isSeaShadow)
    bindAttribute(1, "wave");
}

void ShadowShader::getAllUniformLocations() {
  ShaderProgram::getAllUniformLocations();
  if (isSeaShadow)
    location_time = getUniformLocation("time");
}

void ShadowShader::render() {
  start();
  glm::vec3 lightPos(LIGHT::X, LIGHT::Y, LIGHT::Z);
  glm::mat4 viewMatrix = glm::lookAt(lightPos, glm::vec3(0.0f, -SEA::RADIUS, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera->getZoom()), (float) ACTUAL_WIDTH / (float) ACTUAL_HEIGHT, SHADOW::NEAR_PLANE, SHADOW::FAR_PLANE);
  loadMatrix4f(location_lightSpaceMatrix, projectionMatrix * viewMatrix);
  if (isSeaShadow) {
    loadFloat(location_time, TIMER);
    RawModel* model = SEA_MODEL->getModel();
    loadMatrix4f(location_transformationMatrix, SEA_MODEL->getTransformationMatrix());
    model->bind();

    glDrawElements(GL_TRIANGLES, model->getVertexCount(), GL_UNSIGNED_INT, (void*) 0);
    RawModel::unbind();
  } else {
    for (auto& entry: allEntities) {
      vector<Entity*>* entities = &(entry.second);
      entry.first->bind();
      for (int i = 0; i < entities->size(); ++i) {
        Entity* entity = entities->at(i);
        RawModel* model = entity->getModel();
        loadMatrix4f(location_transformationMatrix, entity->getTransformationMatrix());
        glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());
      }
      RawModel::unbind();
    }
  }

  stop();
}

void ShadowShader::clean() {
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
}

unsigned int ShadowShader::getFboID() {
  return fboID;
}

unsigned int ShadowShader::getDepthMap() {
  return depthMap;
}
