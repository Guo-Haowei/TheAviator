// VelocityShader.h
#pragma once
#include "ShaderProgram.h"

class VelocityShader: public ShaderProgram {
private:
protected:
  int location_PVM;
  int location_prevPVM;

  void bindAttributes();
  void getAllUniformLocations();
public:
  VelocityShader();
  ~VelocityShader();

  void render();
  void updateEntityVelocity();
};