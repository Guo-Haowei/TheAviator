// ShadowShader.cc
#define GLEW_STATIC
#include "ShadowShader.h"
#include "../common.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

unsigned int ShadowShader::fboID;
unsigned int ShadowShader::depthMap;

ShadowShader::ShadowShader(bool isSeaShadow): isSeaShadow(isSeaShadow) {
  if (isSeaShadow) {
    const char* VERTEX_FILE = "../shaders/seaShadow.vert";
    const char* FRAGMENT_FILE = "../shaders/seaShadow.frag";
    ShaderProgram::init(VERTEX_FILE, FRAGMENT_FILE);
  } else {
    const char* VERTEX_FILE = "../shaders/.vert";
    const char* FRAGMENT_FILE = "../shaders/.frag";
    ShaderProgram::init(VERTEX_FILE, FRAGMENT_FILE);
  }
}

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
void ShadowShader::init() {
  glGenFramebuffers(1, &fboID);

  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glBindFramebuffer(GL_FRAMEBUFFER, fboID);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowShader::bindAttributes() {
  bindAttribute(0, "position");
  if (isSeaShadow)
    bindAttribute(1, "wave");
}

void ShadowShader::getAllUniformLocations() {
  location_transformationMatrix = getUniformLocation("transformationMatrix");
  location_projectionMatrix = getUniformLocation("projectionMatrix");
  location_viewMatrix = getUniformLocation("viewMatrix");
  if (isSeaShadow)
    location_time = getUniformLocation("time");
}

void ShadowShader::render() {
  start();
  glm::vec3 lightPos(LIGHT::X, LIGHT::Y, LIGHT::Z);
  // hard code for now
  // loadMatrix4f(location_viewMatrix, glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
  // glm::mat4 projectionMatrix = glm::perspective(90.0f, (float) ACTUAL_WIDTH / (float) ACTUAL_HEIGHT, NEAR_PLANE, FAR_PLANE);
  // loadMatrix4f(location_projectionMatrix, projectionMatrix);
  loadMatrix4f(location_viewMatrix, camera->getViewMatrix());
  glm::mat4 projectionMatrix = glm::perspective(camera->getZoom(), (float) ACTUAL_WIDTH / (float) ACTUAL_HEIGHT, NEAR_PLANE, FAR_PLANE);
  loadMatrix4f(location_projectionMatrix, projectionMatrix);
  if (isSeaShadow) {
    loadFloat(location_time, TIMER);
    RawModel* model = SEA_MODEL->getModel();
    loadMatrix4f(location_transformationMatrix, SEA_MODEL->getTransformationMatrix());
    model->bind();

    glDrawElements(GL_TRIANGLES, model->getVertexCount(), GL_UNSIGNED_INT, (void*) 0);
    RawModel::unbind();
  } else {

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

ShadowShader* ShadowShader::setEntities(vector<Entity*>* entities) {
  this->entities = entities;
  return this;
}

ShadowShader* ShadowShader::setCamera(Camera* camera) {
  this->camera = camera;
  return this;
}

unsigned int ShadowShader::getFboID() {
  return fboID;
}

unsigned int ShadowShader::getDepthMap() {
  return depthMap;
}
