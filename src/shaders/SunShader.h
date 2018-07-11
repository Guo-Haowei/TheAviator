// SunShader.h
#pragma once
#include "ShaderProgram.h"
#include <textures/Texture.h>

class RawModel;

class SunShader: public ShaderProgram {
private:
  Texture m_sunTexture;
  RawModel* quad;

protected:
  int location_sunTexture;
  int location_transform;

  void bindAttributes();
  void getAllUniformLocations();
public:
  SunShader();
  ~SunShader();

  void render();
};