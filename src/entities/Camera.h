// Camera.h
#pragma once
#include <glm/glm.hpp>

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 1.0f;
const float ZOOM = 45.0f;

class Camera {
private:
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 up;
  glm::vec3 right;
  glm::vec3 worldUp;

  float yaw, pitch, zoom;

  void updateCameraVectors();
public:
  Camera();

  void update();
  glm::mat4 getViewMatrix();
  float getZoom() const;
};

extern Camera primaryCamera;
