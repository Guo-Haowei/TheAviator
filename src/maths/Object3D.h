// Object3D.h
#pragma once
#include <glm/glm.hpp>

enum Type {
  UNKNOWN = 0,
  SPHERE,
  BOX,
  TETRAHEDRON,
};

class Object3D {
public:
  Type type;
  glm::vec3 center;

  Object3D();
};

class Sphere: public Object3D {
public:
  float radius;

  Sphere(float radius);
};