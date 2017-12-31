// ParticleHolder.h
#pragma once
#include <entities/DynamicEntity.h>

class ParticleHolder {
private:
  static std::vector<DynamicEntity*> particles;
public:
  ParticleHolder();
  ~ParticleHolder();

  void spawnParticles(glm::vec3 position, int denstiy, glm::vec3 color, float scale);
  void update();

  static ParticleHolder& theOne();
  static std::vector<DynamicEntity*>& getParticles();
};
