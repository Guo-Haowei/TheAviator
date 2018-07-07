// SeaShader.h
#pragma once
#include "ShaderProgram.h"
#include <textures/Texture.h>

class ShadowShader: public ShaderProgram {
private:
  bool isSeaShadow;
  static unsigned int fboID;
  static Texture depthMap;
protected:
  int location_time;
  void bindAttributes();
  void getAllUniformLocations();
public:
  ShadowShader(bool isSeaShadow = false);
  static void init();

  void render();
  void clean();

  static unsigned int getFboID();
  static Texture& getDepthMap();
};
