// Collision.cc
#include "Collision.h"
#include <utils/Debug.h>
#include <maths/Object3D.h>
#include <models/Geometry.h>
#include <entities/DynamicEntity.h>
#include <entities/gameObjects/Airplane.h>
#ifdef __APPLE__
#include <utils/Audio.h>
#endif // __APPLE__
#include <iostream>
#include <vector>
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
#ifdef __APPLE__
          Audio::playAudio("ouch");
        } else {
          Audio::playAudio("tink");
#endif // __APPLE__
        }
        entity->setLifespan(0);
      }
    }
  }
}
