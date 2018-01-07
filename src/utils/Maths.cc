// Maths.cc
#include "Maths.h"
#include <cstdlib>
#include <iostream>
using std::cout;

float Maths::toRadians(float degree) {
  return degree * PI / 180.0f;
}

float Maths::rand(float min, float max) {
  return min + (float)std::rand() / (float)RAND_MAX * (max - min);
}
