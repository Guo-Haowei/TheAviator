// AirPlane.h
#pragma once
#include "Entity.h"
#include <glm/glm.hpp>
#include <vector>

class AirPlane {
private:
  glm::vec3 position;
  std::vector<Entity*> components;

  Entity cockpit;
  Entity engine;
public:
  AirPlane();
  ~AirPlane();

  void update();
};
