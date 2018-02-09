// SeaShader.h
#pragma once
#include "ShaderProgram.h"

class ShadowShader: public ShaderProgram {
private:
  bool isSeaShadow;
  static unsigned int fboID;
  static unsigned int depthMap;
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
  static unsigned int getDepthMap();
};
