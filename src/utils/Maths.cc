// Maths.cc
#include "Maths.h"
#include <cstdlib>
#include <iostream>
using std::cout;

float Maths::rand(float min, float max) {
  return min + (float)std::rand() / (float)RAND_MAX * (max - min);
}
