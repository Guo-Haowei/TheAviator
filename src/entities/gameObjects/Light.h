// Light.h
#pragma once
#include <glm/glm.hpp>

class Light {
private:
  glm::vec3 position;
public:
  Light();
  ~Light();

  void setPosition(float x, float y, float z);
  const glm::vec3& getPosition();
  void update();
  static Light& theOne();
};