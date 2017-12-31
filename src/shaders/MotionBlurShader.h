// MotionBlurShader.h
#pragma once
#include "ShaderProgram.h"

class RawModel;

class MotionBlurShader: public ShaderProgram {
private:
  RawModel* quad;
protected:
  int location_colorTexture;
  int location_velocityTexture;

  void bindAttributes();
  void getAllUniformLocations();
public:
  MotionBlurShader();
  ~MotionBlurShader();

  void render();
};