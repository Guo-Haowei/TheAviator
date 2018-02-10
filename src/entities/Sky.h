// Sky.h
#pragma once
#include <glm/glm.hpp>
#include <vector>

class Entity;

class Sky {
private:
  int cloudCount;
  std::vector<std::vector<Entity*>> clouds;
public:
  Sky();
  ~Sky();

  void createCloud(float angle);
};

