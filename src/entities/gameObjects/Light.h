// Light.h
#pragma once
#include <glm/glm.hpp>
#include <entities/Entity.h>

class Light {
private:
  glm::vec3 position;
  Entity light;
public:
  Light();
  ~Light();

  void setPosition(float x, float y, float z);
  const glm::vec3& getPosition();
  void update();
  static Light& theOne();
};