// UIShader.h
#pragma once
#include "ShaderProgram.h"

class RawModel;

class UIShader: public ShaderProgram {
private:
  RawModel* quad;
  int location_health;
  int location_width;
  int location_height;
public:
  UIShader();
  ~UIShader();

  void bindAttributes();
  void getAllUniformLocations();
  void render();
};