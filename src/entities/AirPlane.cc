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
  engine(Geometry::cube, glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(0.0f), white, glm::vec3(2.0f, 5.0f, 5.0f)),
  tail(Geometry::cube, glm::vec3(-4.0f, 2.0f, 0.0f), glm::vec3(0.0f), red, glm::vec3(1.5f, 2.0f, 0.5f)),
  sideWing(Geometry::cube, glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(0.0f), red, glm::vec3(3.0f, 0.5f, 12.0f)),
  windShield(Geometry::cube, glm::vec3(0.5f, 2.7f, 0.0f), glm::vec3(0.0f), white, glm::vec3(0.3f, 1.5f, 2.0f), 0.3f),
  propeller(Geometry::propeller, glm::vec3(6.0f, 0.0f, 0.0f), glm::vec3(0.0f), brown, glm::vec3(2.0f, 1.0f, 1.0f)),
  blade1(Geometry::cube, glm::vec3(6.8f, 0.0f, 0.0f), glm::vec3(0.0f), brownDark, glm::vec3(0.1f, 8.0f, 1.0f)),
  blade2(Geometry::cube, glm::vec3(6.8f, 0.0f, 0.0f), glm::vec3(0.0f), brownDark, glm::vec3(0.1f, 8.0f, 1.0f)),
  wheelProtectionR(Geometry::cube, glm::vec3(2.5f, -2.0f, 2.5f), glm::vec3(0.0f), red, glm::vec3(3.0f, 1.5f, 1.0f)),
  wheelProtectionL(Geometry::cube, glm::vec3(2.5f, -2.0f, -2.5f), glm::vec3(0.0f), red, glm::vec3(3.0f, 1.5f, 1.0f)),
  tireR(Geometry::cube, glm::vec3(2.5f, -2.8f, 2.5f), glm::vec3(0.0f), brownDark, glm::vec3(2.4f, 2.4f, 0.4f)),
  tireL(Geometry::cube, glm::vec3(2.5f, -2.8f, -2.5f), glm::vec3(0.0f), brownDark, glm::vec3(2.4f, 2.4f, 0.4f)),
  wheelAxis(Geometry::cube, glm::vec3(2.5, -2.8f, 0.0f), glm::vec3(0.0f), brown, glm::vec3(1.0f, 1.0f, 6.0f)),
  suspension(Geometry::cube, glm::vec3(-3.2f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, -0.3f), red, glm::vec3(0.4f, 2.0f, 0.4f)),
  tireB(Geometry::cube, glm::vec3(-3.5f, -0.5f, 0.0f), glm::vec3(0.0f), brownDark, glm::vec3(1.2f, 1.2f, 0.2f)),
  wheelAxisB(Geometry::cube, glm::vec3(-3.5f, -0.5f, 0.0f), glm::vec3(0.0f), brown, glm::vec3(0.5f, 0.5f, 0.3f))
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
  components.push_back(&wheelProtectionR);
  components.push_back(&wheelProtectionL);
  components.push_back(&tireR);
  components.push_back(&tireL);
  components.push_back(&wheelAxis);
  components.push_back(&suspension);
  components.push_back(&tireB);
  components.push_back(&wheelAxisB);

  for (int i = 0; i < components.size(); ++i) {
    allEntities.push_back(components[i]);
    components[i]->changePosition(position);
  }
}

AirPlane::~AirPlane() {}

void AirPlane::update() {
  blade1.changeRotation(10.0f, 0.0f, 0.0f);
  blade2.changeRotation(10.0f, 0.0f, 0.0f);
  propeller.changeRotation(10.0f, 0.0f, 0.0f);
  // for (int i = 0; i < components.size(); ++i) {
  //   components[i]->changePosition(glm::vec3(0.01f, 0.0f, 0.0f));
  // }
}
