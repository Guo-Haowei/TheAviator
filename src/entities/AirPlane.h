// Airplane.h
#pragma once
#include "Entity.h"
#include <glm/glm.hpp>
#include <vector>

class Airplane {
private:
  glm::vec3 position;
  glm::vec4 axisX, axisY, axisZ;
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
  Entity hair[12];
public:
  Airplane();
  ~Airplane();

  void updateHair();
  void rotate(float dx, float dy, float dz, glm::vec3 center);
  void translate(float dx, float dy, float dz);
  void update();
};
