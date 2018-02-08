// EntityShader.h
#pragma once
#include "ShaderProgram.h"
#include "../entities/Camera.h"
#include "../entities/Entity.h"
#include <vector>
using std::vector;

class EntityShader: public ShaderProgram {
private:
  Camera* camera;
  vector<Entity*>* entities;
protected:
  int location_projectionMatrix;
  int location_viewMatrix;
  int location_color;
  int location_light;
  int location_shadowMap;
  int location_opacity;
  void bindAttributes();
  void getAllUniformLocations();
public:
  EntityShader();

  void render();

  EntityShader* setCamera(Camera* camera);
  EntityShader* setEntities(vector<Entity*>* entities);
};
