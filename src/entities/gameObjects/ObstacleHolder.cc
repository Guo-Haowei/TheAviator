// ObstacleHolder.cc
#include "ObstacleHolder.h"
#include <maths/Object3D.h>
#include <common.h>
#include <utils/Debug.h>
#include <maths/Maths.h>
#include <models/Geometry.h>
#include <iostream>

glm::vec3 obstacleColor(RED[0], RED[1], RED[2]);

ObstacleHolder::ObstacleHolder(): lastSpawnDistance(offscreenLeft) {}

ObstacleHolder::~ObstacleHolder() {
  for (auto& obstatcle : obstatcles) {
    delete obstatcle;
  }
}

void ObstacleHolder::spawn(float distance) {
  if (distance >= lastSpawnDistance + miniumDist_O) {
    lastSpawnDistance = distance;
    if (!Maths::chance(spawnChance_O))
      return;
    float h = Maths::rand(minHeight, maxHeight) + SEA::RADIUS;
    glm::vec3 position(h * glm::sin(offscreenLeft), h * glm::cos(offscreenLeft) - SEA::RADIUS, 0.0f);
    float scale = 3.0f;
    DynamicEntity* obstacle = new DynamicEntity(
      OBSTACLE,
      Geometry::sphere,
      position,
      obstacleColor,
      scale
    );
    obstacle->setBody(new Sphere(scale));
    obstacle->setDistance(distance);
    obstatcles.push_back(obstacle);
    DynamicEntity::addEntity(obstacle);
  }
}

void ObstacleHolder::update() {
  spawn(GAME::AIRPLANE_DISTANCE);
  for (int i = 0; i < obstatcles.size(); ++i) {
    if (obstatcles[i]->getDistance() + offscreenRight < GAME::AIRPLANE_DISTANCE || !obstatcles[i]->getLifespan()) {
      delete obstatcles[i];
      obstatcles.erase(obstatcles.begin() + i);
      --i;
    }
  }
  // update rotation
  for (auto& obstatcle : obstatcles) {
    obstatcle->changeRotation(0.0f, 0.05f, 0.0f);
    obstatcle->changeRotation(glm::vec3(0.0f, 0.0f, 1.0f), GAME::SPEED, glm::vec3(0.0f, -SEA::RADIUS, 0.0f));
  }
}

ObstacleHolder& ObstacleHolder::theOne() {
  static ObstacleHolder obstacleHolder;
  return obstacleHolder;
}
