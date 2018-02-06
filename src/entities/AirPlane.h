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
  Entity tail;
  Entity sideWing;
  Entity windShield;
  Entity propeller;
  Entity blade1;
  Entity blade2;
  Entity wheelProtectionR;
  Entity wheelProtectionL;
  Entity tireR;
  Entity tireL;
  Entity wheelAxis;
  Entity suspension;
  Entity wheelB;
  Entity wheelAxisB;
public:
  AirPlane();
  ~AirPlane();

  void update();
};
