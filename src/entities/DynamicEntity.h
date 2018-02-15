// DynamicEntity.h
#pragma once
#include "Entity.h"

#define LIFESPAN 60

class DynamicEntity: public Entity {
private:
  float distance; // distance is the angle relative to plane
  glm::vec3 velocity;
  int lifespan;
  const bool isParticle;
  float originScale;
public:
  DynamicEntity(RawModel* model, glm::vec3 position = glm::vec3(0.0f), glm::vec3 color = glm::vec3(1.0f), float scale = 1.0f, float opacity = 1.0f, bool receiveShadow = true, bool castShadow = true, bool isParticle = false);
  ~DynamicEntity();

  float getDistance() const;
  void setDistance(float distance);
  int getLifespan() const;
  void setLifespan(int lifespan);
  void setVelocity(glm::vec3 velocity);
  void deplete();

  static void addEntity(DynamicEntity* entity);
  static void removeEntity(DynamicEntity* entity);
};

typedef std::map<RawModel*, std::vector<DynamicEntity*>> DynamicEntities;
extern DynamicEntities dynamicEntities;
