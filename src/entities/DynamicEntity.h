// DynamicEntity.h
#pragma once
#include "Entity.h"

class DynamicEntity: public Entity {
private:
  int lifespan;
public:
  DynamicEntity(RawModel* model, glm::vec3 position = glm::vec3(0.0f), glm::vec3 color = glm::vec3(1.0f), glm::vec3 scale = glm::vec3(1.0f), float opacity = 1.0f, bool receiveShadow = true, bool castShadow = true);
};