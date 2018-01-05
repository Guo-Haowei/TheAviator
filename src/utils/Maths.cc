// Maths.cc
#include "Maths.h"
#include <cstdlib>
#include <math.h>

float Maths::PI = 3.141593;

float Maths::toRadians(float degree) {
  return degree * PI / 180.0f;
}

float Maths::sin(float degree) {
  return sin(toRadians(degree));
}

float Maths::cos(float degree) {
  return cos(toRadians(degree));
}

float Maths::rand() {
  return (float)rand() / (float)RAND_MAX;
}

float Maths::rand(float min, float max) {
  return min + rand() * (max - min);
}
