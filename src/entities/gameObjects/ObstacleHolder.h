// ObstacleHolder.h
#pragma once
#include <entities/DynamicEntity.h>

class ObstacleHolder {
private:
  std::vector<DynamicEntity*> obstatcles;
  float lastSpawnDistance;
public:
  ObstacleHolder();
  ~ObstacleHolder();

  void spawn(float distance);
  void update();

  static ObstacleHolder& theOne();
};
