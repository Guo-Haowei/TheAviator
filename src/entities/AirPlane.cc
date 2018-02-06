// AirPlane.cc
#include "AirPlane.h"
#include "../common.h"
#include "../models/Geometry.h"
using std::vector;

glm::vec3 red(RED[0], RED[1], RED[2]);
glm::vec3 white(WHITE[0], WHITE[1], WHITE[2]);
glm::vec3 brown(BROWN[0], BROWN[1], BROWN[2]);
glm::vec3 brownDark(BROWNDARK[0], BROWNDARK[1], BROWNDARK[2]);

AirPlane::AirPlane(): 
  position(glm::vec3(AIRPLANE::X, AIRPLANE::Y, AIRPLANE::Z)),
  cockpit(Geometry::cockpit, glm::vec3(0.0f), glm::vec3(0.0f), red),
  engine(Geometry::cube, glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(0.0f), white, glm::vec3(2.0f, 5.0f, 5.0f)),
  tail(Geometry::cube, glm::vec3(-8.0f, 4.0f, 0.0f), glm::vec3(0.0f), red, glm::vec3(1.5f, 2.0f, 0.5f)),
  sideWing(Geometry::cube, glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f), red, glm::vec3(3.0f, 0.5f, 12.0f)),
  windShield(Geometry::cube, glm::vec3(1.0f, 5.4f, 0.0f), glm::vec3(0.0f), white, glm::vec3(0.3f, 1.5f, 2.0f)),
  propeller(Geometry::propeller, glm::vec3(9.0f, 0.0f, 0.0f), glm::vec3(0.0f), brown, glm::vec3(2.0f, 1.0f, 1.0f)),
  blade1(Geometry::cube, glm::vec3(13.0f, 0.0f, 0.0f), glm::vec3(0.0f), brownDark, glm::vec3(0.1f, 8.0f, 1.0f)),
  blade2(Geometry::cube, glm::vec3(13.0f, 0.0f, 0.0f), glm::vec3(0.0f), brownDark, glm::vec3(0.1f, 8.0f, 1.0f))
{
  blade2.changeRotation(90.0f, 0.0f, 0.0f);
  components.push_back(&cockpit);
  components.push_back(&propeller);
  components.push_back(&engine);
  components.push_back(&tail);
  components.push_back(&sideWing);
  components.push_back(&windShield);
  components.push_back(&blade1);
  components.push_back(&blade2);

  for (int i = 0; i < components.size(); ++i) {
    allEntities.push_back(components[i]);
    components[i]->changePosition(position);
  }
}

AirPlane::~AirPlane() {}

void AirPlane::update() {
  blade1.changeRotation(10.0f, 0.0f, 0.0f);
  blade2.changeRotation(10.0f, 0.0f, 0.0f);
}