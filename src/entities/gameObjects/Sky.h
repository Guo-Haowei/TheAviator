// Sky.h
#pragma once
#include <glm/glm.hpp>
#include <vector>

class Entity;

class Cloud {
private:
  glm::vec3 position;
  std::vector<Entity*> clouds;
  float rotationSpeed;
public:
  Cloud();
  ~Cloud();

  void add(Entity* entity);
  void rotate(float dx, float dy, float dz, glm::vec3 center);
  void translate(float dx, float dy, float dz);
  void rotateEntity();
};

class Sky {
private:
  int cloudCount;
  std::vector<Cloud*> clouds;
public:
  Sky();
  ~Sky();

  void createCloud(float angle);
  void update();

  static Sky& theOne();
};
