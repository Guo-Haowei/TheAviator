// BatteryHolder.h
#pragma once
#include <entities/DynamicEntity.h>

class BatteryHolder {
private:
  std::vector<DynamicEntity*> batteries;
  float lastSpawnDistance;
public:
  BatteryHolder();
  ~BatteryHolder();

  void spawn(float distance);
  void update();

  static BatteryHolder& theOne();
};
