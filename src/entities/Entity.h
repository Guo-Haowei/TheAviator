// Entity.h
#pragma once
#include "../models/RawModel.h"
#include <glm/glm.hpp>
#include <vector>
#include <map>

class Entity {
private:
  RawModel* model;
  float opacity;

  void updateRotation(glm::mat4 rotationMatrix);
protected:
  glm::vec3 position, color, scale;
  glm::mat4 transformation;
public:
  Entity();
  Entity(const Entity& other);
  Entity(RawModel* model, glm::vec3 position = glm::vec3(0.0f), glm::vec3 color = glm::vec3(1.0f), glm::vec3 scale = glm::vec3(1.0f), float opacity = 1.0f);

  glm::vec3 getPosition() const;
  void setPosition(float dx, float dy, float dz);
  void changePosition(float dx, float dy, float dz);

  void changeRotation(float dx, float dy, float dz, glm::vec3 center);
  void changeRotation(glm::mat4 rotationMatrix);
  void changeRotation(glm::vec3 axis, float angle);
  void changeRotation(glm::vec3 axis, float angle, glm::vec3 center);

  glm::vec3 getScale() const;
  void setScale(float dx, float dy, float dz);
  void changeScale(float dx, float dy, float dz);

  glm::mat4 getTransformationMatrix();

  float getOpacity() const;
  RawModel* getModel() const;
  glm::vec3 getColor() const;
  
  static void addEntity(Entity* entity);
};

typedef std::map<RawModel*, std::vector<Entity*>> AllEntities;
extern AllEntities allEntities;
