// Entity.cc
#include "Entity.h"
#include "../utils/Maths.h"
#include <glm/gtc/matrix_transform.hpp>

vector<Entity*> allEntities;

Entity::Entity(): model(nullptr), position(glm::vec3(0.0f)), rotation(glm::vec3(0.0f)), color(glm::vec3(0.0f)), scale(glm::vec3(0.0f)), opacity(1.0f) {}

Entity::Entity(const Entity& other): model(other.model), position(other.position), color(other.color), scale(other.scale), opacity(other.opacity) {}

Entity::Entity(RawModel* model, glm::vec3 position, glm::vec3 color, glm::vec3 scale, float opacity): model(model), position(position), rotation(glm::vec3(0.0f)), color(color), scale(scale), opacity(opacity) {}

glm::mat4 Entity::getTransformationMatrix() {
  glm::mat4 matrix = glm::mat4(1);
  matrix = glm::translate(matrix, position);
  matrix = glm::rotate(matrix, rotation.x, glm::vec3(1, 0, 0));
  matrix = glm::rotate(matrix, rotation.y, glm::vec3(0, 1, 0));
  matrix = glm::rotate(matrix, rotation.z, glm::vec3(0, 0, 1));
  matrix = glm::scale(matrix, scale);
  return matrix;
}

void Entity::changePosition(float dx, float dy, float dz) {
  position.x += dx;
  position.y += dy;
  position.z += dz;
}

void Entity::changePosition(glm::vec3 xyz) {
  position += xyz;
}

void Entity::setPosition(float dx, float dy, float dz) {
  position = glm::vec3(dx, dy, dz);
}

void Entity::changeRotation(float dx, float dy, float dz) {
  rotation.x += Maths::toRadians(dx);
  rotation.y += Maths::toRadians(dy);
  rotation.z += Maths::toRadians(dz);
}

void Entity::changeRotation(glm::vec3 xyz) {
  rotation.x += Maths::toRadians(xyz.x);
  rotation.y += Maths::toRadians(xyz.y);
  rotation.z += Maths::toRadians(xyz.z);
}

void Entity::changeScale(float dx, float dy, float dz) {
  scale.x += dx;
  scale.y += dy;
  scale.z += dz;
}

void Entity::setScale(float dx, float dy, float dz) {
  scale = glm::vec3(dx, dy, dz);
}

void Entity::changeScale(glm::vec3 xyz) {
  scale += xyz;
}

glm::vec3 Entity::getPosition() const {
  return position;
}

glm::vec3 Entity::getRotation() const {
  return rotation;
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
