// Entity.h
#pragma once
#include "../models/RawModel.h"
#include <glm/glm.hpp>
#include <vector>
using std::vector;

class Entity {
private:
  RawModel* model;
  glm::vec3 position, rotation, color, scale;
  float opacity;
public:
  Entity(RawModel* model, glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 color = glm::vec3(1.0f), glm::vec3 scale = glm::vec3(1.0f), float opacity = 1.0f);

  glm::mat4 getTransformationMatrix();
  void changePosition(float dx, float dy, float dz);
  void changePosition(glm::vec3 xyz);
  void changeRotation(float dx, float dy, float dz);
  void changeRotation(glm::vec3 xyz);

  glm::vec3 getPosition() const;
  glm::vec3 getRotation() const;
  glm::vec3 getColor() const;
  glm::vec3 getScale() const;
  float getOpacity() const;
  RawModel* getModel() const;
};

extern vector<Entity*> allEntities;
