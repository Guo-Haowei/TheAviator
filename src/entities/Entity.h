// Entity.h
#pragma once
#include "../models/RawModel.h"
#include <glm/glm.hpp>

class Entity {
private:
  RawModel* model;
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 color;
  float scale;
public:
  Entity(RawModel* model, glm::vec3 position, glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 color = glm::vec3(1.0f), float scale = 1.0f);

  void changePosition(float dx, float dy, float dz);
  void changeRotation(float dx, float dy, float dz);

  glm::vec3 getPosition() const;
  glm::vec3 getRotation() const;
  glm::vec3 getColor() const;
  float getScale() const;
  RawModel* getModel() const;
};
