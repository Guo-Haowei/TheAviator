// CollisionShader.h
#pragma once
#include "ShaderProgram.h"
#include <entities/DynamicEntity.h>
#include <vector>

class CollisionShader: public ShaderProgram {
protected:
  Camera* camera;
  int location_projectionMatrix;
  int location_viewMatrix;
  int location_color;
  void bindAttributes();
  void getAllUniformLocations();
public:
  CollisionShader();

  void render(const std::vector<DynamicEntity*>& entities);

  static CollisionShader& theOne();
};
