// Camera.h
#pragma once
#include <glm/glm.hpp>

const float YAW = -90.0f;
const float PITCH = 0.0f;

class Camera {
private:
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 up;

  float fov;
public:
  Camera();

  void update();
  void changePosition(float degree);
  glm::mat4 getProjectionMatrix();
  glm::mat4 getViewMatrix();
  glm::mat4 getPVMatrix();

  glm::mat4 getLightSpaceMatrix();

  float getFov() const;
  void setFov(float fov);
  glm::vec3 getPosition() const;
  void chasePoint(glm::vec3 position);

  glm::vec2 screenPos(glm::vec4 worldPos);

  static Camera& primary();
};
