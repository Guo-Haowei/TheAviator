// ObstacleHolder.cc
#include "ObstacleHolder.h"
#include <common.h>
#include <utils/Debug.h>
#include <utils/Maths.h>
#include <models/Geometry.h>
#include <iostream>

// extract to common later
const float offscreenLeft = 0.5f;
const float offscreenRight = 1.0f;
const float miniumDist = 0.1f;
const float spawnChance = 0.3f;
const float minHeight = 20.0f;
const float maxHeight = 60.0f;

glm::vec3 obstacleColor(RED[0], RED[1], RED[2]);

ObstacleHolder::ObstacleHolder(): lastSpawnDistance(offscreenLeft) {}

ObstacleHolder::~ObstacleHolder() {
  for (auto& obstatcle : obstatcles) {
    delete obstatcle;
  }
}

void ObstacleHolder::spawn(float distance) {
  if (distance >= lastSpawnDistance + miniumDist) {
    lastSpawnDistance = distance;
    if (!Maths::chance(spawnChance))
      return;
    float h = Maths::rand(minHeight, maxHeight) + SEA::RADIUS;
    glm::vec3 position(h * glm::sin(offscreenLeft), h * glm::cos(offscreenLeft) - SEA::RADIUS, 0.0f);
    DynamicEntity* obstacle = new DynamicEntity(Geometry::sphere, position, obstacleColor, glm::vec3(2.0f));
    obstacle->setDistance(distance);
    obstatcles.push_back(obstacle);
    DynamicEntity::addEntity(obstacle);
  }
}

void ObstacleHolder::update() {
  spawn(GAME::AIRPLANE_DISTANCE);
  // check if need to delete
  for (int i = 0; i < obstatcles.size(); ++i) {
    if (obstatcles[i]->getDistance() + offscreenRight < GAME::AIRPLANE_DISTANCE) {
      DynamicEntity* entity = obstatcles[i];
      DynamicEntity::removeEntity(entity);
      obstatcles.erase(obstatcles.begin() + i);
      --i;
    }
  }
  // update rotation
  for (auto& obstatcle : obstatcles) {
    obstatcle->changeRotation(0.0f, 0.01f, 0.0f);
    obstatcle->changeRotation(glm::vec3(0.0f, 0.0f, 1.0f), GAME::SPEED, glm::vec3(0.0f, -SEA::RADIUS, 0.0f));
  }
}