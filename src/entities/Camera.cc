// Camera.cc
#include "Camera.h"
#include "../common.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
using std::cout;

Camera primaryCamera;

Camera::Camera() {
  position = glm::vec3(0.0f);
  up = glm::vec3(0.0f, 1.0f, 0.0f);
  worldUp = up;
  front = glm::vec3(0.0f, 0.0f, -1.0f);
  yaw = YAW;
  pitch = PITCH;
  zoom = 45.0f;
  updateCameraVectors();
}

void Camera::initPrimaryCamera() {
  primaryCamera.setPosition(glm::vec3(CAMERA::X, CAMERA::Y, CAMERA::Z));
  primaryCamera.setZoom(CAMERA::ZOOM);
}

void Camera::update() {
}

glm::mat4 Camera::getViewMatrix() {
  return glm::lookAt(position, position + front, up);
}

void Camera::updateCameraVectors() {
  right = glm::normalize(glm::cross(front, worldUp));
  up = glm::normalize(glm::cross(right, front));
}

float Camera::getZoom() const {
  return zoom;
}

void Camera::setZoom(float zoom) {
  this->zoom = zoom;
}

void Camera::setPosition(glm::vec3 position) {
  this->position = position;
}
