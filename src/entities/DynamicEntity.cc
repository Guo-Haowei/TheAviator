// DynamicEntity.cc
#include "DynamicEntity.h"
#include <iostream>
using std::cout;

DynamicEntity::DynamicEntity(RawModel* model, glm::vec3 position, glm::vec3 color, glm::vec3 scale, float opacity, bool receiveShadow, bool castShadow) : Entity(model, position, color, scale, opacity, receiveShadow, castShadow) {
  transformation = glm::mat4(1.0f);
  isStatic = false;
  transformation[3].x = position.x;
  transformation[3].y = position.y;
  transformation[3].z = position.z;
}
