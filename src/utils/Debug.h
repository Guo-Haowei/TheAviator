// Debug.h
#pragma once
#include <glm/glm.hpp>
#include <string>

namespace Debug {
  void printVector(glm::vec2 vec, std::string name = "");
  void printVector(glm::vec3 vec, std::string name = "");
  void printVector(glm::vec4 vec, std::string name = "");
  void printMatrix(glm::mat4 mat, std::string name = "");
};
