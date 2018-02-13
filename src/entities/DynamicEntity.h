// DynamicEntity.h
#pragma once
#include "Entity.h"

class DynamicEntity: public Entity {
private:
  float distance; // distance is the angle relative to plane
  int lifespan;
  bool expired;
public:
  DynamicEntity(RawModel* model, glm::vec3 position = glm::vec3(0.0f), glm::vec3 color = glm::vec3(1.0f), glm::vec3 scale = glm::vec3(1.0f), float opacity = 1.0f, bool receiveShadow = true, bool castShadow = true);

  float getDistance() const;
  void setDistance(float distance);
  bool getExpired() const;
  void setExpired(bool expired);

  static void addEntity(DynamicEntity* entity);
  static void removeEntity(DynamicEntity* entity);
};

typedef std::map<RawModel*, std::vector<DynamicEntity*>> DynamicEntities;
extern DynamicEntities dynamicEntities;
