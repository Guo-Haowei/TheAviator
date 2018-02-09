// EntityShader.h
#pragma once
#include "ShaderProgram.h"

class EntityShader: public ShaderProgram {
private:
  Camera* camera;
protected:
  int location_projectionMatrix;
  int location_viewMatrix;
  int location_color;
  int location_light;
  int location_shadowMap;
  int location_opacity;
  void bindAttributes();
  void getAllUniformLocations();
public:
  EntityShader();

  void render();
};
