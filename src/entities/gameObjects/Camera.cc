// Camera.cc
#include "Camera.h"
#include <common.h>
#include <maths/Maths.h>
#include <io/MouseManager.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
using std::cout;

Camera primaryCamera;

Camera::Camera() {
  position = glm::vec3(0.0f);
  up = glm::vec3(0.0f, 1.0f, 0.0f);
  worldUp = up;
  front = glm::vec3(0.0f, 0.0f, -1.0f);
  fov = 45.0f;
  updateCameraVectors();
}

void Camera::initPrimaryCamera() {
  primaryCamera.setPosition(glm::vec3(CAMERA::X, CAMERA::Y, CAMERA::Z));
  primaryCamera.setFov(CAMERA::FOV);
}

void Camera::update() {
  fov = Maths::clamp(MouseManager::getX(), -1.0f, 1.0f, 40.0f, 80.0f);
}

glm::mat4 Camera::getProjectionMatrix() {
  return glm::perspective(glm::radians(getFov()), (float) ACTUAL_WIDTH / (float) ACTUAL_HEIGHT, NEAR_PLANE, FAR_PLANE);
}

glm::mat4 Camera::getViewMatrix() {
  return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getLightSpaceMatrix() {
  glm::vec3 lightPos(LIGHT::X, LIGHT::Y, LIGHT::Z);
  glm::mat4 viewMatrix = glm::lookAt(lightPos, glm::vec3(AIRPLANE::X, AIRPLANE::Y, AIRPLANE::Z), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 projectionMatrix = glm::perspective(glm::radians(getFov()), (float) ACTUAL_WIDTH / (float) ACTUAL_HEIGHT, SHADOW::NEAR_PLANE, SHADOW::FAR_PLANE);
  return projectionMatrix * viewMatrix;
}

void Camera::updateCameraVectors() {
  right = glm::normalize(glm::cross(front, worldUp));
  up = glm::normalize(glm::cross(right, front));
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

void Camera::setPosition(glm::vec3 position) {
  this->position = position;
}

void Camera::chasePoint(glm::vec3 position) {
  float delta = (position.y - this->position.y) * 0.1f;
  this->position.y += delta;
}
