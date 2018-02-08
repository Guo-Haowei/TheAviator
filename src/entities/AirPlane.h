// AirPlane.h
#pragma once
#include "Entity.h"
#include <glm/glm.hpp>
#include <vector>

class AirPlane {
private:
  glm::vec3 position;
  std::vector<Entity*> components;

  // plane
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
  Entity tireB;
  Entity wheelAxisB;

  // pilot
  Entity body;
  Entity face;
  Entity hairSide;
  Entity hairBack;
public:
  AirPlane();
  ~AirPlane();

  void update();
};
