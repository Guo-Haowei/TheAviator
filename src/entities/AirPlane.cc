// Airplane.cc
#include "Airplane.h"
#include "../common.h"
#include "../utils/Maths.h"
#include "../models/Geometry.h"
#include "../utils/Debug.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
using std::vector;

glm::vec3 red(RED[0], RED[1], RED[2]);
glm::vec3 white(WHITE[0], WHITE[1], WHITE[2]);
glm::vec3 brown(BROWN[0], BROWN[1], BROWN[2]);
glm::vec3 brownDark(BROWNDARK[0], BROWNDARK[1], BROWNDARK[2]);
glm::vec3 pink(PINK[0], PINK[1], PINK[2]);

Airplane::Airplane() :
  position(glm::vec3(0.0f)),
  axisX(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)),
  axisY(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)),
  axisZ(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)),
  cockpit(Geometry::cockpit, glm::vec3(0.0f), red),
  engine(Geometry::cube, glm::vec3(5.0f, 0.0f, 0.0f), white, glm::vec3(2.0f, 5.0f, 5.0f)),
  tail(Geometry::cube, glm::vec3(-4.0f, 2.0f, 0.0f), red, glm::vec3(1.5f, 2.0f, 0.5f)),
  sideWing(Geometry::cube, glm::vec3(0.0f, 1.5f, 0.0f), red, glm::vec3(3.0f, 0.5f, 12.0f)),
  windShield(Geometry::cube, glm::vec3(0.5f, 2.7f, 0.0f), white, glm::vec3(0.3f, 1.5f, 2.0f), 0.3f),
  propeller(Geometry::propeller, glm::vec3(6.0f, 0.0f, 0.0f), brown, glm::vec3(2.0f, 1.0f, 1.0f)),
  blade1(Geometry::cube, glm::vec3(6.8f, 0.0f, 0.0f), brownDark, glm::vec3(0.1f, 8.0f, 1.0f)),
  blade2(Geometry::cube, glm::vec3(6.8f, 0.0f, 0.0f), brownDark, glm::vec3(0.1f, 8.0f, 1.0f)),
  wheelProtectionR(Geometry::cube, glm::vec3(2.5f, -2.0f, 2.5f), red, glm::vec3(3.0f, 1.5f, 1.0f)),
  wheelProtectionL(Geometry::cube, glm::vec3(2.5f, -2.0f, -2.5f), red, glm::vec3(3.0f, 1.5f, 1.0f)),
  tireR(Geometry::cube, glm::vec3(2.5f, -2.8f, 2.5f), brownDark, glm::vec3(2.4f, 2.4f, 0.4f)),
  tireL(Geometry::cube, glm::vec3(2.5f, -2.8f, -2.5f), brownDark, glm::vec3(2.4f, 2.4f, 0.4f)),
  wheelAxis(Geometry::cube, glm::vec3(2.5, -2.8f, 0.0f), brown, glm::vec3(1.0f, 1.0f, 6.0f)),
  suspension(Geometry::cube, glm::vec3(-3.2f, 0.5f, 0.0f), red, glm::vec3(0.4f, 2.0f, 0.4f)),
  tireB(Geometry::cube, glm::vec3(-3.5f, -0.5f, 0.0f), brownDark, glm::vec3(1.2f, 1.2f, 0.2f)),
  wheelAxisB(Geometry::cube, glm::vec3(-3.5f, -0.5f, 0.0f), brown, glm::vec3(0.5f, 0.5f, 0.3f)),
  body(Geometry::cube, glm::vec3(-0.8f, 1.5f, 0.0f), brown, glm::vec3(1.5f)),
  face(Geometry::cube, glm::vec3(-1.0f, 2.7f, 0.0f), pink, glm::vec3(1.0f)),
  hairSide(Geometry::cube, glm::vec3(-1.3f, 3.0f, 0.0f), brown, glm::vec3(1.2f, 0.4f, 1.2f)),
  hairBack(Geometry::cube, glm::vec3(-1.6f, 2.8f, 0.0f), brown, glm::vec3(0.2f, 0.8f, 1.0f))
{
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
  components.push_back(&body);
  components.push_back(&face);
  components.push_back(&hairSide);
  components.push_back(&hairBack);

  // create hair
  for (int i = 0; i < 12; ++i) {
    int col = i % 3;
    int row = i / 3;
    float startX = -1.9f;
    float startY = 3.2f;
    float startZ = -0.4f;
    hair[i] = Entity(Geometry::cube, glm::vec3(startX + (float)row * 0.4f, startY, startZ + (float)col * 0.4f), brown , glm::vec3(0.4f));
    components.push_back(&(hair[i]));
  }

  for (int i = 0; i < components.size(); ++i) {
    Entity::addEntity(components[i]);
  }

  suspension.changeRotation(glm::vec3(0.0f, 0.0f, 1.0f), -0.3f);
  blade2.changeRotation(axisX, glm::radians(90.0f));
  translate(AIRPLANE::X, AIRPLANE::Y, AIRPLANE::Z);
}

Airplane::~Airplane() {}

void Airplane::rotate(float dx, float dy, float dz, glm::vec3 center) {
  float angle = dx != 0.0f ? dx : dy != 0.0f ? dy : dz;
  glm::mat4 rotationMatrix = Maths::calculateRotationMatrix(dx, dy, dz, center);
  axisX = rotationMatrix * axisX;
  axisY = rotationMatrix * axisY;
  axisZ = rotationMatrix * axisZ;
  for (int i = 0; i < components.size(); ++i) {
    components[i]->changeRotation(rotationMatrix);
  }
}

void Airplane::translate(float dx, float dy, float dz) {
  position += glm::vec3(dx, dy, dz);
  glm::mat4 translationMatrix = Maths::calculateTranslationMatrix(dx, dy, dz);
  for (int i = 0; i < components.size(); ++i) {
    components[i]->changePosition(translationMatrix);
  }
}

void Airplane::updateHair() {
  static float hairAngle = 0.0f;
  for (int i = 0; i < 12; ++i) {
    float height = 0.3f + glm::cos(hairAngle + i / 3) * 0.1f;
    float dy = (height - hair[i].getScale().y) / 2;
    glm::vec3 translateVector = dy * glm::normalize(glm::vec3(axisY.x, axisY.y, axisY.z));
    hair[i].changePosition(translateVector.x, translateVector.y, translateVector.z);
    hair[i].setScale(0.4f, height, 0.4f);
  }
  hairAngle += 0.16f;
}

void Airplane::update() {

  //rotate(0.0, 0.0, glm::radians(1.0f), position);
  //translate(0.1f, 0.1f, 0.0f);

  // update hair
  updateHair();
  // update propeller
  blade1.changeRotation(axisX, glm::radians(10.0f));
  blade2.changeRotation(axisX, glm::radians(10.0f));
  propeller.changeRotation(axisX, glm::radians(10.0f));
} 