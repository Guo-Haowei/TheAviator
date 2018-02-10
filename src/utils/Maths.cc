// Maths.cc
#include "Maths.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>
#include <iostream>
using std::cout;

float Maths::rand(float min, float max) {
  return min + (float)std::rand() / (float)RAND_MAX * (max - min);
}

glm::mat4 Maths::calculateRotationMatrix(glm::vec3 axis, float angle, glm::vec3 center) {
  glm::mat4 rotationMatrix, T, T_1;

  float yz = sqrt(axis.y * axis.y + axis.z * axis.z);
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
  T = glm::translate(T, -center);
  T_1 = glm::translate(T_1, center);
  return T_1 * rotationMatrix * T;
}

glm::mat4 Maths::calculateRotationMatrix(glm::vec3 axis, float cosTheta, float sinTheta, glm::vec3 center) {
  glm::mat4 rotationMatrix, T, T_1;

  float yz = sqrt(axis.y * axis.y + axis.z * axis.z);
  glm::mat4 Rx = glm::mat4(1.0f), RxInverse = glm::mat4(1.0f), Ry = glm::mat4(1.0f), RyInverse = glm::mat4(1.0f), Rz = glm::mat4(1.0f);
  float sinTheta1 = axis.y / yz, cosTheta1 = axis.z / yz;
  Rx[1].y = cosTheta1; Rx[1].z = sinTheta1; Rx[2].y = -sinTheta1; Rx[2].z = cosTheta1;
  RxInverse[1].y = cosTheta1; RxInverse[1].z = -sinTheta1; RxInverse[2].y = sinTheta1; RxInverse[2].z = cosTheta1;
  float sinTheta2 = -axis.x, cosTheta2 = yz;
  Ry[0].x = cosTheta2; Ry[0].z = -sinTheta2; Ry[2].x = sinTheta2; Ry[2].z = cosTheta2;
  RyInverse[0].x = cosTheta2; RyInverse[0].z = sinTheta2; RyInverse[2].x = -sinTheta2; RyInverse[2].z = cosTheta2;

  Rz[0].x = cosTheta; Rz[0].y = sinTheta; Rz[1].x = -sinTheta; Rz[1].y = cosTheta;

  rotationMatrix = RxInverse * RyInverse * Rz * Ry * Rx;
  T = glm::translate(T, -center);
  T_1 = glm::translate(T_1, center);
  return T_1 * rotationMatrix * T;
}