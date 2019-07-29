// Maths.cc
#include "Maths.h"
#include <utils/Debug.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cstdlib>
#include <iostream>
using std::cout;

int Maths::rand(int min, int max) {
  return min + std::rand() * (float)(max - min) / (float)RAND_MAX;
}

float Maths::rand(float min, float max) {
  return min + (float)std::rand() / (float)RAND_MAX * (max - min);
}

float Maths::clamp(float low, float value, float high) {
  return std::max(low, std::min(value, high));
}

float Maths::clamp(float value, float low, float high, float clampLow, float clampHigh) {
  float boundedValue = std::max(low, std::min(value, high));
  return (boundedValue - low) / (high - low) * (clampHigh - clampLow) + clampLow;
}

bool Maths::chance(float chance) {
  return chance >= rand(0.0f, 1.0f);
}

glm::mat4 Maths::calculateTranslationMatrix(float x, float y, float z) {
  glm::vec3 delta = glm::vec3(x, y, z);
  glm::mat4 translation(1.0f);
  translation = glm::translate(translation, delta);
  return translation;
}

glm::mat4 Maths::calculateRotationMatrix(float x, float y, float z, glm::vec3 center) {
  glm::mat4 rotationMatrix(1.0f), T(1.0f), T_1(1.0f);
  T = glm::translate(T, -center);
  T_1 = glm::translate(T_1, center);

  rotationMatrix = glm::rotate(rotationMatrix, x, glm::vec3(1.0f, 0.0f, 0.0f));
  rotationMatrix = glm::rotate(rotationMatrix, y, glm::vec3(0.0f, 1.0f, 0.0f));
  rotationMatrix = glm::rotate(rotationMatrix, z, glm::vec3(0.0f, 0.0f, 1.0f));

  rotationMatrix = T_1 * rotationMatrix * T;
  return rotationMatrix;
}

glm::mat4 Maths::rotateAroundAxis(glm::vec3 axis, float angle, glm::vec3 center) {
  glm::mat4 rotationMatrix(1.0f), T(1.0f), T_1(1.0f);
  T = glm::translate(T, -center);
  T_1 = glm::translate(T_1, center);

  rotationMatrix = glm::rotate(rotationMatrix, angle, axis);

  return T_1 * rotationMatrix * T;
}
