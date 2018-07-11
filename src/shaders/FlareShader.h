// FlareShader.h
#pragma once
#include "ShaderProgram.h"
#include <textures/Texture.h>
#define TOTAL_FLARE_TEXTURE 15

class RawModel;

class FlareShader: public ShaderProgram {
private:
  typedef struct FlareTexture {
    int index;
    float scale;
  } FlareTexture;
  Texture m_textures[9];
  FlareTexture m_flareTextures[TOTAL_FLARE_TEXTURE];
  RawModel* quad;

protected:
  int location_flareTexture;
  int location_brightness;
  int location_transform;

  void bindAttributes();
  void getAllUniformLocations();
public:
  FlareShader();
  ~FlareShader();

  void render();
};