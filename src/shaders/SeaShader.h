// SeaShader.h
#pragma once
#include "ShaderProgram.h"

class SeaShader: public ShaderProgram {
protected:
  int location_projectionMatrix;
  int location_viewMatrix;
  int location_time;
  int location_light;
  int location_shadowMap;
  void bindAttributes();
  void getAllUniformLocations();
public:
  SeaShader();

  void render();

  virtual ~SeaShader();
};
