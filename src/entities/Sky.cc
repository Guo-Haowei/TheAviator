// Sky.cc
#include "Sky.h"
#include "Entity.h"
#include "../common.h"
#include "../utils/Maths.h"
#include "../models/Geometry.h"
#include <iostream>
using std::vector;

glm::vec3 color_near(WHITE[0], WHITE[1], WHITE[2]);
glm::vec3 color_far(BACKGROUND_COLOR2[0], BACKGROUND_COLOR2[1], BACKGROUND_COLOR2[2]);

Sky::Sky(): cloudCount(20) {
  float stepAngle = PI * 2 / cloudCount;
  for (int i = 0; i < cloudCount; ++i) {
    createCloud(stepAngle * (float)i);
  }
}

Sky::~Sky() {
  for (auto& cloud: clouds) {
    for (int i = 0; i < cloud.size(); ++i) {
      delete cloud[i];
    }
  }
}

void Sky::createCloud(float angle) {
  int index = clouds.size();
  clouds.push_back(vector<Entity*>());

  int nBlocks = 3 + Maths::rand(0, 3);
  float height = Maths::rand(60.0f, 140.0f) + SEA::RADIUS;
  float cloudScale = Maths::rand(1.0f, 3.0f);
  for (int i = 0; i < nBlocks; ++i) {
    glm::vec3 position((float)i * 6.0f * cloudScale, Maths::rand(0.0f, 4.0f), Maths::rand(0.0f, 4.0f));
    float scale = 8.0f * Maths::rand(0.2f, 0.9f) * cloudScale;
    Entity* entity = new Entity(Geometry::cube, position, glm::vec3(0.0f), glm::vec3(scale), 1.0f, false, true);

    clouds[index].push_back(entity);
    entity->changeRotation(0.0f, Maths::rand(0.0f, 2 * PI), Maths::rand(0.0f, 2.0f * PI));

    Entity::addEntity(entity);
    entity->changePosition(glm::cos(angle) * height, glm::sin(angle) * height - SEA::RADIUS, Maths::rand(-320.0f, -120.0f));
    // linear fog color
    static float near = -100.0f;
    static float far = -320.0f;
    static float dist = entity->getPosition().z;
    float fogFactor = (far - dist) / (far - near);
    glm::vec3 adjustedColor = (1.0f - fogFactor) * color_far + fogFactor * color_near;
    entity->setColor(adjustedColor);
  }
}

void Sky::update() {
  for (auto& cloud: clouds) {
    for (int i = 0; i < cloud.size(); ++i) {
      // rotate around world
      cloud[i]->changeRotation(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(0.1f), glm::vec3(0.0f, -SEA::RADIUS, 0.0f));
      // rotate around self
      cloud[i]->changeRotation(0.0f, Maths::rand(0.0f, 0.005f) * (float)(i + 1), Maths::rand(0.0f, 0.002) * (float)(i + 1));
    }
  }
}