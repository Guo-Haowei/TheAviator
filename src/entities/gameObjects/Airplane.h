// Airplane.h
#pragma once
#include <models/Geometry.h>
#include <entities/Entity.h>
#include <vector>

class Entity;

class Airplane {
private:
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec4 axisX, axisY, axisZ;
  std::vector<Entity*> components;
  float speed;

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
  Entity& getBody();
  void knockBack(glm::vec3 otherPosition);

  static Airplane& theOne();
  static std::vector<Entity*> rigidBody;
};
