// Entity.cc
#include "Entity.h"
#include "../utils/Maths.h"
#include <glm/gtc/matrix_transform.hpp>
using std::vector;

Entity::Entity(): model(nullptr), position(glm::vec3(0.0f)), color(glm::vec3(0.0f)), scale(glm::vec3(0.0f)), opacity(1.0f) {
  transformation[3].x = position.x;
  transformation[3].y = position.y;
  transformation[3].z = position.z;
}

Entity::Entity(const Entity& other): model(other.model), position(other.position), color(other.color), scale(other.scale), opacity(other.opacity) {
  transformation[3].x = position.x;
  transformation[3].y = position.y;
  transformation[3].z = position.z;
}

Entity::Entity(RawModel* model, glm::vec3 position, glm::vec3 color, glm::vec3 scale, float opacity) : model(model), position(position), color(color), scale(scale), opacity(opacity) {
  transformation[3].x = position.x;
  transformation[3].y = position.y;
  transformation[3].z = position.z;
}

glm::mat4 Entity::getTransformationMatrix() {
  glm::mat4 scaleMatrix(1);
  scaleMatrix = glm::scale(scaleMatrix, scale);
  return transformation * scaleMatrix;
}

void Entity::changePosition(float dx, float dy, float dz) {
  glm::vec3 delta = glm::vec3(dx, dy, dz);
  glm::mat4 translation(1);
  translation = glm::translate(translation, delta);
  transformation = translation * transformation;

  position += delta;
}

void Entity::setPosition(float dx, float dy, float dz) {
  position = glm::vec3(dx, dy, dz);
  transformation[3].x = dx;
  transformation[3].y = dy;
  transformation[3].z = dz;
}

void Entity::changeRotation(float dx, float dy, float dz) {
  glm::mat4 rotationMatrix(1);
  glm::mat4 T, T_1;
  T = glm::translate(T, -position);
  T_1 = glm::translate(T_1, position);
  rotationMatrix = glm::rotate(rotationMatrix, glm::radians(dx), glm::vec3(1, 0, 0));
  rotationMatrix = glm::rotate(rotationMatrix, glm::radians(dy), glm::vec3(0, 1, 0));
  rotationMatrix = glm::rotate(rotationMatrix, glm::radians(dz), glm::vec3(0, 0, 1));

  transformation = T_1 * rotationMatrix * T * transformation;
  position.x = transformation[3].x;
  position.y = transformation[3].y;
  position.z = transformation[3].z;
}

void Entity::changeRotation(float dx, float dy, float dz, glm::vec3 center) {
  glm::mat4 rotationMatrix(1);
  glm::mat4 T, T_1;
  T = glm::translate(T, -center);
  T_1 = glm::translate(T_1, center);
  rotationMatrix = glm::rotate(rotationMatrix, glm::radians(dx), glm::vec3(1, 0, 0));
  rotationMatrix = glm::rotate(rotationMatrix, glm::radians(dy), glm::vec3(0, 1, 0));
  rotationMatrix = glm::rotate(rotationMatrix, glm::radians(dz), glm::vec3(0, 0, 1));

  transformation = T_1 * rotationMatrix * T * transformation;
  position.x = transformation[3].x;
  position.y = transformation[3].y;
  position.z = transformation[3].z;
}

void Entity::changeScale(float dx, float dy, float dz) {
  scale.x += dx;
  scale.y += dy;
  scale.z += dz;
}

void Entity::setScale(float dx, float dy, float dz) {
  scale = glm::vec3(dx, dy, dz);
}

glm::vec3 Entity::getPosition() const {
  return position;
}

glm::vec3 Entity::getColor() const {
  return color;
}

glm::vec3 Entity::getScale() const {
  return scale;
}

float Entity::getOpacity() const {
  return opacity;
}

RawModel* Entity::getModel() const {
  return model;
}

AllEntities allEntities;

void Entity::addEntity(Entity* entity) {
  RawModel* key = entity->getModel();
  if (allEntities.find(key) == allEntities.end())
    allEntities.insert(std::pair<RawModel*, vector<Entity*>>(key, vector<Entity*>()));
  allEntities.at(key).push_back(entity);
}
