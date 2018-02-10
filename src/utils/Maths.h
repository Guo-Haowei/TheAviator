// Maths.h
#pragma once
#include <glm/glm.hpp>
#define PI 3.14159265358979323846

namespace Maths {
  int rand(int min, int max);
  float rand(float min, float max);

  glm::mat4 calculateTranslationMatrix(float x, float y, float z);
  glm::mat4 calculateRotationMatrix(float x, float y, float z, glm::vec3 center);
  glm::mat4 rotateAroundAxis(glm::vec3 axis, float angle, glm::vec3 center);
};
