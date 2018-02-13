// Object3D.cc
#include "Object3D.h"

Object3D::Object3D(): type(UNKNOWN), center(glm::vec3(0.0f)) {}

Sphere::Sphere(float radius): radius(radius) {
  type = SPHERE;
}