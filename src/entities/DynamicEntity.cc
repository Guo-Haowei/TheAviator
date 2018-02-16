// DynamicEntity.cc
#include "DynamicEntity.h"
#include <models/Geometry.h>
#include <maths/Maths.h>
#include <entities/gameObjects/ParticleHolder.h>
#include <common.h>
#include <iostream>
using std::cout;
using std::vector;

DynamicEntity::DynamicEntity(
  EntityType type,
  RawModel* model,
  glm::vec3 position,
  glm::vec3 color,
  float scale,
  float opacity,
  bool receiveShadow,
  bool castShadow):
  type(type),
  lifespan(1),
  distance(0.0f),
  originScale(scale),
  Entity(model, position, color, glm::vec3(scale), opacity, receiveShadow, castShadow)
{}

DynamicEntity::~DynamicEntity() {
  if (type) {
    removeEntity(this);
    // generate particle effects
    float density = type == OBSTACLE ? 15 : 8;
    ParticleHolder::theOne().spawnParticles(position, density, color, (float) density * scale.x / 15.0f);
  }
}

float DynamicEntity::getDistance() const {
  return distance;
}

void DynamicEntity::setDistance(float distance) {
  this->distance = distance;
}

int DynamicEntity::getLifespan() const {
  return lifespan;
}

void DynamicEntity::setLifespan(int lifespan) {
  this->lifespan = lifespan;
}

void DynamicEntity::deplete() {
  // change scale
  scale = glm::vec3(originScale * (float)lifespan / (float)LIFESPAN);
  // change velocity and position
  velocity += glm::vec3(velocity.x > 0.0f ? -0.04f: 0.04f, -0.06f, 0.0f);
  changePosition(velocity.x, velocity.y, velocity.z);
  // rotate randomly
  changeRotation(Maths::rand(0.0f, 12.0f), Maths::rand(0.0f, 12.0f), 0.0f);
  if (lifespan)
    lifespan--;
}

void DynamicEntity::setVelocity(glm::vec3 velocity) {
  this->velocity = velocity;
}

EntityType DynamicEntity::getType() const {
  return type;
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
