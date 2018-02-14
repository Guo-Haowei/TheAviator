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

float Maths::clamp(float value, float low, float high, float clampLow, float clampHigh) {
  float boundedValue = std::max(low, std::min(value, high));
  return (boundedValue - low) / (high - low) * (clampHigh - clampLow) + clampLow;
}

bool Maths::chance(float chance) {
  return chance >= rand(0.0f, 1.0f);
}

glm::mat4 Maths::calculateTranslationMatrix(float x, float y, float z) {
  glm::vec3 delta = glm::vec3(x, y, z);
  glm::mat4 translation(1);
  translation = glm::translate(translation, delta);
  return translation;
}

glm::mat4 Maths::calculateRotationMatrix(float x, float y, float z, glm::vec3 center) {
  glm::mat4 rotationMatrix, T, T_1;
  T = glm::translate(T, -center);
  T_1 = glm::translate(T_1, center);

  rotationMatrix = glm::rotate(rotationMatrix, x, glm::vec3(1.0f, 0.0f, 0.0f));
  rotationMatrix = glm::rotate(rotationMatrix, y, glm::vec3(0.0f, 1.0f, 0.0f));
  rotationMatrix = glm::rotate(rotationMatrix, z, glm::vec3(0.0f, 0.0f, 1.0f));

  rotationMatrix = T_1 * rotationMatrix * T;
  return rotationMatrix;
}

glm::mat4 Maths::rotateAroundAxis(glm::vec3 axis, float angle, glm::vec3 center) {
  glm::mat4 rotationMatrix, T, T_1;
  T = glm::translate(T, -center);
  T_1 = glm::translate(T_1, center);

  axis = glm::normalize(axis);
  float yz = sqrt(axis.y * axis.y + axis.z * axis.z);
  float xz = sqrt(axis.x * axis.x + axis.z * axis.z);
  if (yz != 0.0f) {
    glm::mat4 Rx = glm::mat4(1.0f), RxInverse = glm::mat4(1.0f), Ry = glm::mat4(1.0f), RyInverse = glm::mat4(1.0f), Rz = glm::mat4(1.0f);
    float sinTheta1 = axis.y / yz, cosTheta1 = axis.z / yz;
    Rx[1].y = cosTheta1; Rx[1].z = sinTheta1; Rx[2].y = -sinTheta1; Rx[2].z = cosTheta1;
    RxInverse[1].y = cosTheta1; RxInverse[1].z = -sinTheta1; RxInverse[2].y = sinTheta1; RxInverse[2].z = cosTheta1;

    float sinTheta2 = -axis.x, cosTheta2 = yz;
    Ry[0].x = cosTheta2; Ry[0].z = -sinTheta2; Ry[2].x = sinTheta2; Ry[2].z = cosTheta2;
    RyInverse[0].x = cosTheta2; RyInverse[0].z = sinTheta2; RyInverse[2].x = -sinTheta2; RyInverse[2].z = cosTheta2;

    float cosTheta = glm::cos(angle), sinTheta = glm::sin(angle);
    Rz[0].x = cosTheta; Rz[0].y = sinTheta; Rz[1].x = -sinTheta; Rz[1].y = cosTheta;

    rotationMatrix = RxInverse * RyInverse * Rz * Ry * Rx;
  } else {
    rotationMatrix = glm::rotate(rotationMatrix, angle, axis);
  }

  return T_1 * rotationMatrix * T;
}
