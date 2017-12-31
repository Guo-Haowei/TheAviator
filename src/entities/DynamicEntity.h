// DynamicEntity.h
#pragma once
#include "Entity.h"

const int LIFESPAN = 60;
const float offscreenLeft = 0.5f;
const float offscreenRight = 1.5f;
const float miniumDist_O = 0.1f;
const float miniumDist_B = 0.3f;
const float spawnChance_O = 0.3f;
const float spawnChance_B = 0.5f;
const float minHeight = 20.0f;
const float maxHeight = 60.0f;

enum EntityType {
  PARTICLE = 0,
  BATTERY,
  OBSTACLE
};

class DynamicEntity: public Entity {
private:
  float distance; // distance is the angle relative to plane
  glm::vec3 velocity;
  int lifespan;
  const EntityType type;
  float originScale;
public:
  DynamicEntity(
    EntityType type,
    RawModel* model,
    glm::vec3 position = glm::vec3(0.0f),
    glm::vec3 color = glm::vec3(1.0f),
    float scale = 1.0f,
    float opacity = 1.0f,
    bool receiveShadow = true,
    bool castShadow = true
  );
  ~DynamicEntity();

  float getDistance() const;
  void setDistance(float distance);
  int getLifespan() const;
  void setLifespan(int lifespan);
  void setVelocity(glm::vec3 velocity);
  void deplete();
  EntityType getType() const;

  static void addEntity(DynamicEntity* entity);
  static void removeEntity(DynamicEntity* entity);
};

typedef std::map<RawModel*, std::vector<DynamicEntity*>> DynamicEntities;
extern DynamicEntities dynamicEntities;
