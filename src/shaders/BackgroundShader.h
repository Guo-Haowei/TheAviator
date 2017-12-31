// BackgroundShader.h
#pragma once
#include "ShaderProgram.h"

class RawModel;

class BackgroundShader: public ShaderProgram {
private:
  RawModel* background;
public:
  BackgroundShader();
  ~BackgroundShader();

  void bindAttributes();
  void getAllUniformLocations();
  void render();
};
