// Camera.cc
#include "Camera.h"
#include "Light.h"
#include <common.h>
#include <io/KeyboardManager.h>
#include <maths/Maths.h>
#include <io/MouseManager.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
using std::cout;

Camera::Camera() {
  position = glm::vec3(CAMERA::X, CAMERA::Y, CAMERA::Z);
  up = glm::vec3(0.0f, 1.0f, 0.0f);
  front = glm::vec3(0.0f, 0.0f, -1.0f);
  fov = CAMERA::FOV;
}

void Camera::changePosition(float degree) {
  glm::mat4 r, t, t_1;
  t = glm::translate(t, glm::vec3(0.0f, -position.y, 0.0f));
  t_1 = glm::translate(t_1, glm::vec3(0.0f, position.y, 0.0f));
  r = glm::rotate(r, degree, glm::vec3(0.0f, 1.0f, 0.0f));
  r = t_1 * r * t;
  position = glm::vec3(r * glm::vec4(position, 1.0f));
  front = glm::vec3(r * glm::vec4(front, 1.0f));
}

void Camera::update() {
  float delta = (1.0f - 2 * MouseManager::getRawX() / (float) WIDTH);
  float z = Maths::clamp(delta, -1.0f, 1.0f, CAMERA::Z - 10.0f, CAMERA::Z + 80.0f);
  position.z = z;
  if (KeyboardManager::isKeyDown(KEY_LEFT)) {
    changePosition(-0.03f);
  }
  if (KeyboardManager::isKeyDown(KEY_RIGHT)) {
    changePosition(0.03f);
  }
}

glm::mat4 Camera::getProjectionMatrix() {
  return glm::perspective(glm::radians(getFov()), (float) ACTUAL_WIDTH / (float) ACTUAL_HEIGHT, NEAR_PLANE, FAR_PLANE);
}

glm::mat4 Camera::getViewMatrix() {
  return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getPVMatrix() {
  return getProjectionMatrix() * getViewMatrix();
}

glm::mat4 Camera::getLightSpaceMatrix() {
  glm::vec3 lightPos(Light::theOne().getPosition());
  glm::mat4 viewMatrix = glm::lookAt(lightPos, glm::vec3(AIRPLANE::X, AIRPLANE::Y, AIRPLANE::Z), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 projectionMatrix = glm::perspective(glm::radians(getFov()), (float) ACTUAL_WIDTH / (float) ACTUAL_HEIGHT, SHADOW::NEAR_PLANE, SHADOW::FAR_PLANE);
  return projectionMatrix * viewMatrix;
}

float Camera::getFov() const {
  return fov;
}

void Camera::setFov(float fov) {
  this->fov = fov;
}

glm::vec3 Camera::getPosition() const {
  return position;
}

void Camera::chasePoint(glm::vec3 position) {
  float delta = (position.y - this->position.y) * 0.1f;
  this->position.y += delta;
  const float LOWEST = 3.0f;
  if (this->position.y <= LOWEST) {
    this->position.y = LOWEST;
  }
}

glm::vec2 Camera::screenPos(glm::vec4 worldPos) {
  glm::vec4 screenPos = getProjectionMatrix() * getViewMatrix() * worldPos;
  float x = (1.0f + screenPos.x) / 2.0f * (float)ACTUAL_WIDTH;
  float y = (1.0f - screenPos.y) / 2.0f * (float)ACTUAL_HEIGHT;
  return glm::vec2(x, y);
}

Camera& Camera::primary() {
  static Camera primary;
  return primary;
}
