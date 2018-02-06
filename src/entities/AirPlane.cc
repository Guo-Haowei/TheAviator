// AirPlane.cc
#include "AirPlane.h"
#include "../common.h"
#include "../models/Geometry.h"
using std::vector;

AirPlane::AirPlane(): 
  position(glm::vec3(AIRPLANE::X, AIRPLANE::Y, AIRPLANE::Z)),
  cockpit(Geometry::cockpit, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(RED[0], RED[1], RED[2])),
  engine(Geometry::cube, glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(WHITE[0], WHITE[1], WHITE[2]), glm::vec3(2.0f, 5.0f, 5.0f))
{
  components.push_back(&cockpit);
  components.push_back(&engine);

  for (int i = 0; i < components.size(); ++i) {
    allEntities.push_back(components[i]);
    components[i]->changePosition(position);
  }
}

AirPlane::~AirPlane() {}

void AirPlane::update() {}