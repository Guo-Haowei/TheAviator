// Entity.cc
#include "Entity.h"
#include "../utils/Maths.h"

Entity::Entity(RawModel* model, glm::vec3 position, glm::vec3 rotation, glm::vec3 color, glm::vec3 scale): model(model), position(position), rotation(rotation), color(color), scale(scale) { }

void Entity::changePosition(float dx, float dy, float dz) {
  position.x += dx;
  position.y += dy;
  position.z += dz;
}

void Entity::changeRotation(float dx, float dy, float dz) {
  rotation.x += Maths::toRadians(dx);
  rotation.y += Maths::toRadians(dy);
  rotation.z += Maths::toRadians(dz);
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

RawModel* Entity::getModel() const {
  return model;
}
