// Collision.cc
#include "Collision.h"
#include <utils/Debug.h>
#include <maths/Object3D.h>
#include <models/Geometry.h>
#include <entities/DynamicEntity.h>
#include <entities/gameObjects/Airplane.h>
#include <iostream>
#include <vector>
using std::vector;

bool overlap(Object3D* o1, Object3D* o2) {
  if (!o1 || !o2)
    return false;

  if (o1->type == SPHERE && o1->type == SPHERE) {
    std::cout << "checking...\n";
    Debug::printVector(o1->center, "obstacle");
    Debug::printVector(o2->center, "plane");
    float distance = glm::length(o1->center - o2->center);
    return distance - (static_cast<Sphere*>(o1)->radius + static_cast<Sphere*>(o2)->radius) < 0;
  }
  return false;
}

void Collision::checkCollisionAgainstPlane() {
  // check all dynamic entities except particles
  const vector<DynamicEntity*> obstacles = dynamicEntities[Geometry::sphere];
  for (auto& obstacle : obstacles) {
    if (overlap(obstacle->getBody(), Airplane::theOne().getBody())) {
      obstacle->setExpired(true);
    }
  }
}
