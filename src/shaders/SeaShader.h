// SeaShader.h
#pragma once
#include "ShaderProgram.h"
#include "../entities/Camera.h"
#include "../entities/Entity.h"
#include <vector>
using std::vector;

class SeaShader: public ShaderProgram {
private:
  Camera* camera;
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
  void clean();

  SeaShader* setCamera(Camera* camera);
};
