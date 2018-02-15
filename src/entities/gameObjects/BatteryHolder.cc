// BatteryHolder.cc
#include "BatteryHolder.h"
#include <common.h>
#include <maths/Object3D.h>
#include <maths/Maths.h>
#include <models/Geometry.h>
#include <iostream>
using std::vector;

glm::vec3 batteryColor(BLUE[0], BLUE[1], BLUE[2]);

BatteryHolder::BatteryHolder() {}

BatteryHolder::~BatteryHolder() {
  for (auto& battery : batteries) {
    delete battery;
  }
}

void BatteryHolder::spawn(float distance) {
  if (distance >= lastSpawnDistance + miniumDist_B) {
    lastSpawnDistance = distance;
    if (!Maths::chance(spawnChance_B))
      return;
    int batteryNumber = 1 + Maths::rand(0, 10);
    float h = Maths::rand(minHeight, maxHeight) + SEA::RADIUS;
    for (int i = 0; i < batteryNumber; ++i) {
      float angle = offscreenLeft + i * 0.02f;
      float height = h + glm::cos((float)i * 0.2f) * 5.0f;
      glm::vec3 position(height * glm::sin(angle), height * glm::cos(angle) - SEA::RADIUS, 0.0f);
      float scale = 2.0f;
      DynamicEntity* battery = new DynamicEntity(
        BATTERY,
        Geometry::tetrahedron,
        position,
        batteryColor,
        scale
      );
      battery->changeRotation(i * 0.1f, i * 0.1f, 0.0f);
      battery->setBody(new Sphere(scale));
      battery->setDistance(distance + i * 0.03f);
      batteries.push_back(battery);
      DynamicEntity::addEntity(battery);
    }
  }
}

void BatteryHolder::update() {
  spawn(GAME::AIRPLANE_DISTANCE);
  for (int i = 0; i < batteries.size(); ++i) {
    if (batteries[i]->getDistance() + offscreenRight < GAME::AIRPLANE_DISTANCE || !batteries[i]->getLifespan()) {
      delete batteries[i];
      batteries.erase(batteries.begin() + i);
      --i;
    }
  }
  // update rotation
  for (auto& battery : batteries) {
    battery->changeRotation(0.0f, 0.05f, 0.0f);
    battery->changeRotation(glm::vec3(0.0f, 0.0f, 1.0f), GAME::SPEED, glm::vec3(0.0f, -SEA::RADIUS, 0.0f));
  }
}

BatteryHolder& BatteryHolder::theOne() {
  static BatteryHolder batteryHolder;
  return batteryHolder;
}
