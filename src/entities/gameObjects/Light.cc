// Light.cc
#include "Light.h"
#include <models/Geometry.h>
#include <glm/gtc/matrix_transform.hpp>

Light::Light() {}

Light::~Light() {}

void Light::setPosition(float x, float y, float z) {
  position.x = x;
  position.y = y;
  position.z = z;
}

const glm::vec3& Light::getPosition() {
  return position;
}

void Light::update() {
  // glm::mat4 t, t_1, r;
  // glm::vec3 axis(0.0f, position.y, 0.0f);
  // t = glm::translate(t, -axis);
  // t_1 = glm::translate(t_1, axis);
  // r = glm::rotate(r, 0.003f, axis);
  // position = t_1 * (r * (t * glm::vec4(position, 1.0f)));
  // light.setPosition(position.x, position.y, position.z);
}

Light& Light::theOne() {
  static Light light;
  return light;
}
