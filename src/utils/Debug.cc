// Debug.cc
#include "Debug.h"
#include <iostream>
using std::cerr;

void Debug::printVector(glm::vec3 vec) {
  cerr << "[" << vec.x << " " << vec.y << " " << vec.z << "]\n";
}

void Debug::printVector(glm::vec4 vec) {
  cerr << "[" << vec.x << " " << vec.y << " " << vec.z << " " << vec.w << "]\n";
}

void Debug::printMatrix(glm::mat4 mat) {
  cerr << "[" << mat[0].x << " " << mat[1].x << " " << mat[2].x << " " << mat[3].x << "]\n";
  cerr << "[" << mat[0].y << " " << mat[1].y << " " << mat[2].y << " " << mat[3].y << "]\n";
  cerr << "[" << mat[0].z << " " << mat[1].z << " " << mat[2].z << " " << mat[3].z << "]\n";
  cerr << "[" << mat[0].w << " " << mat[1].w << " " << mat[2].w << " " << mat[3].w << "]\n";
}
