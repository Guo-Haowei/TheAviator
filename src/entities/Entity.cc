// Entity.cc
#include "Entity.h"
#include <utils/Maths.h>
#include <utils/Debug.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
using std::vector;

static int ID = 0;

Entity::Entity(): model(nullptr), position(glm::vec3(0.0f)), color(glm::vec3(0.0f)), scale(glm::vec3(0.0f)), opacity(1.0f), receiveShadow(true), castShadow(true) {
  id = ID++;
  transformation = glm::mat4(1.0f);
  isStatic = true;
  transformation[3].x = position.x;
  transformation[3].y = position.y;
  transformation[3].z = position.z;
}

Entity::Entity(const Entity& other): model(other.model), position(other.position), color(other.color), scale(other.scale), opacity(other.opacity), receiveShadow(other.receiveShadow), castShadow(other.castShadow) {
  id = ID++;
  isStatic = true;
  transformation = glm::mat4(1.0f);
  transformation[3].x = position.x;
  transformation[3].y = position.y;
  transformation[3].z = position.z;
}

Entity::Entity(RawModel* model, glm::vec3 position, glm::vec3 color, glm::vec3 scale, float opacity, bool receiveShadow, bool castShadow) : model(model), position(position), color(color), scale(scale), opacity(opacity), receiveShadow(receiveShadow), castShadow(castShadow) {
  id = ID++;
  isStatic = true;
  transformation = glm::mat4(1.0f);
  transformation[3].x = position.x;
  transformation[3].y = position.y;
  transformation[3].z = position.z;
}

void Entity::updateTransformation(glm::mat4 transformationMatrix) {
  transformation = transformationMatrix * transformation;
  position.x = transformation[3].x;
  position.y = transformation[3].y;
  position.z = transformation[3].z;
}

glm::mat4 Entity::getTransformationMatrix() {
  glm::mat4 scaleMatrix(1);
  scaleMatrix = glm::scale(scaleMatrix, scale);
  return transformation * scaleMatrix;
}

void Entity::changePosition(glm::mat4 translationMatrix) {
  updateTransformation(translationMatrix);
}

void Entity::changePosition(float dx, float dy, float dz) {
  glm::vec3 delta = glm::vec3(dx, dy, dz);
  glm::mat4 translationMatrix(1);
  translationMatrix = glm::translate(translationMatrix, delta);
  updateTransformation(translationMatrix);
}

void Entity::setPosition(float dx, float dy, float dz) {
  position = glm::vec3(dx, dy, dz);
  transformation[3].x = dx;
  transformation[3].y = dy;
  transformation[3].z = dz;
}

void Entity::changeRotation(float x, float y, float z) {
  updateTransformation(Maths::calculateRotationMatrix(x, y, z, position));
}

void Entity::changeRotation(glm::mat4 rotationMatrix) {
  updateTransformation(rotationMatrix);
}

void Entity::changeRotation(glm::vec3 axis, float angle) {
  updateTransformation(Maths::rotateAroundAxis(axis, angle, position));
}

void Entity::changeRotation(glm::vec3 axis, float angle, glm::vec3 center) {
  updateTransformation(Maths::rotateAroundAxis(axis, angle, center));
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

void Entity::setColor(glm::vec3 color) {
  this->color = color;
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

bool Entity::getReceiveShadow() const {
  return receiveShadow;
}

bool Entity::getCastShadow() const {
  return castShadow;
}

bool Entity::getIsStatic() const {
  return isStatic;
}

AllEntities allEntities;

void Entity::addEntity(Entity* entity) {
  RawModel* key = entity->getModel();
  if (allEntities.find(key) == allEntities.end())
    allEntities.insert(std::pair<RawModel*, vector<Entity*>>(key, vector<Entity*>()));
  allEntities.at(key).push_back(entity);
}
