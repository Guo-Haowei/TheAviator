// BackgroundShader.h
#pragma once
#include "ShaderProgram.h"
#include "../models/RawModel.h"

class BackgroundShader: public ShaderProgram {
private:
  RawModel* background;
public:
  BackgroundShader();
  ~BackgroundShader();

  RawModel* getModel();
  void bindAttributes();
  void getAllUniformLocations();
};
