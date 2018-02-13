// DynamicEntity.cc
#include "DynamicEntity.h"
#include <models/Geometry.h>
#include <common.h>
#include <iostream>
using std::cout;
using std::vector;

DynamicEntity::DynamicEntity(
  RawModel* model,
  glm::vec3 position,
  glm::vec3 color,
  glm::vec3 scale,
  float opacity,
  bool receiveShadow,
  bool castShadow):
  expired(false),
  distance(0.0f),
  Entity(model, position, color, scale, opacity, receiveShadow, castShadow)
{}

DynamicEntity::~DynamicEntity() {
  // particle effects
  //removeEntity(this);
}

float DynamicEntity::getDistance() const {
  return distance;
}

void DynamicEntity::setDistance(float distance) {
  this->distance = distance;
}

bool DynamicEntity::getExpired() const {
  return expired;
}

void DynamicEntity::setExpired(bool expired) {
  this->expired = expired;
}

DynamicEntities dynamicEntities;

void DynamicEntity::addEntity(DynamicEntity* entity) {
  RawModel* key = entity->getModel();
  if (dynamicEntities.find(key) == dynamicEntities.end())
    dynamicEntities.insert(std::pair<RawModel*, vector<DynamicEntity*>>(key, vector<DynamicEntity*>()));
  dynamicEntities.at(key).push_back(entity);
}

void DynamicEntity::removeEntity(DynamicEntity* entity) {
  RawModel* key = entity->getModel();
  if (dynamicEntities.find(key) != dynamicEntities.end()) {
    vector<DynamicEntity*>& entities = dynamicEntities[key];
    for (int i = 0; i < entities.size(); ++i) {
      if (entities[i]->getId() == entity->getId()) {
        entities.erase(entities.begin() + i);
        break;
      }
    }
  }
}
