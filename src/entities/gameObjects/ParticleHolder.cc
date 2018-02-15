// ParticleHolder.cc
#include "ParticleHolder.h"
#include <maths/Object3D.h>
#include <maths/Maths.h>
#include <models/Geometry.h>
using std::vector;

vector<DynamicEntity*> ParticleHolder::particles;

ParticleHolder::ParticleHolder() {}

ParticleHolder::~ParticleHolder() {}

void ParticleHolder::spawnParticles(glm::vec3 position, int density, glm::vec3 color, float scale) {
  for (int i = 0; i < density; ++i) {
    DynamicEntity* particle = new DynamicEntity(
      PARTICLE,
      Geometry::tetrahedron,
      position,
      color,
      Maths::rand(0.4f, 0.7f) * scale,
      1.0f,
      false,
      true
    );
    particle->setLifespan(LIFESPAN);
    particle->setBody(new Sphere(scale));
    glm::vec3 velocity = glm::vec3(Maths::rand(-1.2f, 1.4f), Maths::rand(-0.5f, 1.5f), 0.0f);
    particle->setVelocity(velocity);
    particles.push_back(particle);
  }
}

void ParticleHolder::update() {
  for (int i = 0; i < particles.size(); ++i) {
    DynamicEntity* particle = particles[i];
    particle->deplete();
    if (particle->getLifespan())
      continue;
    // remove particle
    delete particle;
    particles.erase(particles.begin() + i);
    --i;
  }
}

ParticleHolder& ParticleHolder::theOne() {
  static ParticleHolder particleHolder;
  return particleHolder;
}

vector<DynamicEntity*>& ParticleHolder::getParticles() {
  return particles;
}
