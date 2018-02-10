// Sky.cc
#include "Sky.h"
#include "Entity.h"
#include "../common.h"
#include "../utils/Debug.h"
#include "../utils/Maths.h"
#include "../models/Geometry.h"
#include <iostream>
using std::vector;

glm::vec3 color(WHITE[0], WHITE[1], WHITE[2]);

Sky::Sky(): cloudCount(20) {
  float stepAngle = PI * 2 / cloudCount;
  for (int i = 0; i < cloudCount; ++i) {
    createCloud(stepAngle * (float)i);
  }
}

Sky::~Sky() {}

void Sky::createCloud(float angle) {
  int index = clouds.size();
  clouds.push_back(vector<Entity*>());

  int nBlocks = 3 + Maths::rand(0, 3);
  float height = Maths::rand(15.0f, 35.0f) + SEA::RADIUS;
  for (int i = 0; i < nBlocks; ++i) {
    glm::vec3 position((float)i * 1.5f, Maths::rand(0.0f, 1.0f), Maths::rand(0.0f, 1.0f));
    float scale = 2.0f * Maths::rand(0.1f, 0.9f);
    Entity* entity = new Entity(Geometry::cube, position, color, glm::vec3(scale));

    clouds[index].push_back(entity);
    entity->changeRotation(0.0f, Maths::rand(0.0f, 2 * PI), Maths::rand(0.0f, 2 * PI));

    Entity::addEntity(entity);
    entity->changePosition(glm::cos(angle) * height, glm::sin(angle) * height - SEA::RADIUS, Maths::rand(-80.0f, -30.0f));
  }
}
