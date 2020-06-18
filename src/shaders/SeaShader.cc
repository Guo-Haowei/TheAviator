// SeaShader.cc
#include "SeaShader.h"
#include "glPrerequisites.h"
#include <common.h>
#include <entities/Entity.h>
#include <entities/gameObjects/Light.h>
#include <entities/gameObjects/Camera.h>
#include <models/Geometry.h>
#include <utils/Debug.h>
#include <iostream>
using std::cout;

Entity* SEA_MODEL;

SeaShader::SeaShader() {
  SEA_MODEL = new Entity(Geometry::sea, glm::vec3(0.0f, -SEA::RADIUS, 0.0f));
  SEA_MODEL->changeRotation(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(90.0f));

  const char* VERTEX_FILE = "../shaders/sea.vert";
  const char* FRAGMENT_FILE = "../shaders/sea.frag";
  const char* GEOMETRY_FILE = "../shaders/sea.geom";
  ShaderProgram::init(VERTEX_FILE, FRAGMENT_FILE, GEOMETRY_FILE);
}

SeaShader::~SeaShader() {
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
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glm::vec3 lightPos(Light::theOne().getPosition());
  loadFloat(location_ambientLightIntensity, AMBIENT_LIGHT_INTENSITY);
  loadInt(location_shadowMap, 0);
  loadFloat(location_time, TIMER);
  loadVector3f(location_light, lightPos);
  loadMatrix4f(location_lightSpaceMatrix, Camera::primary().getLightSpaceMatrix());
  loadMatrix4f(location_viewMatrix, Camera::primary().getViewMatrix());
  loadMatrix4f(location_projectionMatrix, Camera::primary().getProjectionMatrix());
  RawModel* model = SEA_MODEL->getModel();
  loadMatrix4f(location_transformationMatrix, SEA_MODEL->getTransformationMatrix());
  model->bind();

  glDrawElements(GL_TRIANGLES, model->getVertexCount(), GL_UNSIGNED_INT, (void*) 0);

  RawModel::unbind();
  stop();

  // update sea
  SEA_MODEL->changeRotation(glm::vec3(0.0f, 0.0f, 1.0f), GAME::SPEED);
}
