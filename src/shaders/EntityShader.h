// EntityShader.h
#pragma once
#include "ShaderProgram.h"

class EntityShader: public ShaderProgram {
protected:
  int location_projectionMatrix;
  int location_viewMatrix;
  int location_color;
  int location_light;
  int location_shadowMap;
  int location_opacity;
  int location_receiveShadow;
  int location_prevPVM;
  void bindAttributes();
  void getAllUniformLocations();
public:
  EntityShader();

  void render();
  void updateEntityVelocity();
};
