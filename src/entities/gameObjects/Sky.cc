// Sky.cc
#include "Sky.h"
#include <entities/Entity.h>
#include <common.h>
#include <maths/Maths.h>
#include <models/Geometry.h>
#include <iostream>
using std::vector;

Cloud::Cloud() {
  rotationSpeed = Maths::rand(0.0f, 0.004f);
}

Cloud::~Cloud() {
  for (auto& cloud : clouds) {
    delete cloud;
  }
}

void Cloud::add(Entity* entity) {
  clouds.push_back(entity);
}

void Cloud::rotate(float dx, float dy, float dz, glm::vec3 center) {
  float angle = dx != 0.0f ? dx : dy != 0.0f ? dy : dz;
  glm::mat4 rotationMatrix = Maths::calculateRotationMatrix(dx, dy, dz, center);
  for (auto& cloud: clouds) {
    cloud->changeRotation(rotationMatrix);
  }
}

void Cloud::translate(float dx, float dy, float dz) {
  position += glm::vec3(dx, dy, dz);
  glm::mat4 translationMatrix = Maths::calculateTranslationMatrix(dx, dy, dz);
  for (auto& cloud: clouds) {
    cloud->changePosition(translationMatrix);
  }
}

void Cloud::rotateEntity() {
  for (int i = 0; i < clouds.size(); ++i) {
    clouds[i]->changeRotation(0.0f, rotationSpeed * (float)(i + 1), rotationSpeed * (float)(i + 1));
  }
}

Sky::Sky(): cloudCount(20) {
  float stepAngle = PI * 2 / cloudCount;
  for (int i = 0; i < cloudCount; ++i) {
    createCloud(stepAngle * (float)i);
  }
}

Sky::~Sky() {
  for (auto& cloud: clouds) {
    delete cloud;
  }
}

glm::vec3 cloudColor(WHITE[0], WHITE[1], WHITE[2]);

void Sky::createCloud(float angle) {
  float height = Maths::rand(60.0f, 140.0f) + SEA::RADIUS;
  Cloud* cloud = new Cloud();
  clouds.push_back(cloud);

  int index = clouds.size();
  int nBlocks = 3 + Maths::rand(0, 3);
  float cloudScale = Maths::rand(1.5f, 2.5f);
  for (int i = 0; i < nBlocks; ++i) {
    glm::vec3 position((float)i * 5.0f * cloudScale, Maths::rand(0.0f, 4.0f), Maths::rand(0.0f, 4.0f));
    float scale = 8.0f * Maths::rand(0.5f, 0.9f) * cloudScale;
    Entity* entity = new Entity(Geometry::cube, position, cloudColor, glm::vec3(scale), 1.0f, false, false);
    entity->changeRotation(0.0f, Maths::rand(0.0f, 2 * PI), Maths::rand(0.0f, 2.0f * PI));

    cloud->add(entity);
    Entity::addEntity(entity);
  }

  glm::vec3 cloudPos(glm::cos(angle) * height, glm::sin(angle) * height - SEA::RADIUS, Maths::rand(-320.0f, -120.0f));
  cloud->translate(cloudPos.x, cloudPos.y, cloudPos.z);
  cloud->rotate(0.0f, 0.0f, angle + PI / 2.0f, cloudPos);
}

void Sky::update() {
  for (auto& cloud: clouds) {
    cloud->rotate(0.0f, 0.0f, GAME::SPEED, glm::vec3(0.0f, -SEA::RADIUS, 0.0f));
    cloud->rotateEntity();
  }
}

Sky& Sky::theOne() {
  static Sky sky;
  return sky;
}
