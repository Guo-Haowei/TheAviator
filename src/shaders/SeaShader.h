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
  Entity* sea;
protected:
  // uniform locations
  int location_transformationMatrix;
  int location_projectionMatrix;
  int location_viewMatrix;
  int location_time;
  void bindAttributes();
  void getAllUniformLocations();
public:
  SeaShader();

  void render();
  void clean();

  SeaShader* setCamera(Camera* camera);
};
