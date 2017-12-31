// Collision.cc
#include "Collision.h"
#include <common.h>
#include <utils/Debug.h>
#include <maths/Object3D.h>
#include <models/Geometry.h>
#include <entities/DynamicEntity.h>
#include <entities/gameObjects/Airplane.h>
#include <iostream>
#include <vector>
#include <algorithm>
using std::vector;

bool overlap(Object3D* o1, Object3D* o2) {
  if (!o1 || !o2)
    return false;

  if (o1->type == SPHERE && o1->type == SPHERE) {
    float distance = glm::length(o1->center - o2->center);
    return distance - (static_cast<Sphere*>(o1)->radius + static_cast<Sphere*>(o2)->radius) < 0;
  }
  return false;
}

void Collision::checkCollisionAgainstPlane() {
  // check all dynamic entities except particles
  for (auto& entry: dynamicEntities) {
    vector<DynamicEntity*>& entities = entry.second;
    for (int i = 0; i < entities.size(); ++i) {
      DynamicEntity* entity = entities[i];
      if (entity->getType() && overlap(entity->getBody(), Airplane::theOne().getBody().getBody())) {
        if (entity->getType() == OBSTACLE) {
          Airplane::theOne().knockBack(entity->getPosition());
          GAME::HEALTH = std::max(0.0f, GAME::HEALTH - 10.0f);
        } else {
          GAME::HEALTH = std::min(100.0f, GAME::HEALTH + 1.0f);
        }
        entity->setLifespan(0);
      }
    }
  }
}
