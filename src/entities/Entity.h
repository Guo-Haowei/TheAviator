// Entity.h
#pragma once
#include "../models/RawModel.h"
#include <glm/glm.hpp>
#include <vector>
#include <map>

class Entity {
private:
  RawModel* model;
  glm::vec3 position, rotation, color, scale;
  float opacity;
public:
  Entity();
  Entity(const Entity& other);
  Entity(RawModel* model, glm::vec3 position = glm::vec3(0.0f), glm::vec3 color = glm::vec3(1.0f), glm::vec3 scale = glm::vec3(1.0f), float opacity = 1.0f);

  glm::mat4 getTransformationMatrix();
  void changePosition(float dx, float dy, float dz);
  void changePosition(glm::vec3 xyz);
  void setPosition(float dx, float dy, float dz);
  void changeRotation(float dx, float dy, float dz);
  void changeRotation(glm::vec3 xyz);
  void changeScale(float dx, float dy, float dz);
  void changeScale(glm::vec3 xyz);
  void setScale(float dx, float dy, float dz);

  glm::vec3 getPosition() const;
  glm::vec3 getRotation() const;
  glm::vec3 getColor() const;
  glm::vec3 getScale() const;
  float getOpacity() const;
  RawModel* getModel() const;
  
  static void addEntity(Entity* entity);
};

typedef std::map<RawModel*, std::vector<Entity*>> AllEntities;
extern AllEntities allEntities;
