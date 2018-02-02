// SeaShader.h
#pragma once
#include "ShaderProgram.h"
#include "../entities/Camera.h"
#include "../entities/Entity.h"
#include <vector>
using std::vector;

class ShadowShader: public ShaderProgram {
private:
  bool isSeaShadow;
  static unsigned int fboID;
  static unsigned int depthMap;
  Camera* camera;
  std::vector<Entity*>* entities;
protected:
  int location_transformationMatrix;
  int location_projectionMatrix;
  int location_viewMatrix;
  int location_time;
  void bindAttributes();
  void getAllUniformLocations();
public:
  ShadowShader(bool isSeaShadow = false);
  static void init();

  void render();
  void clean();

  ShadowShader* setEntities(vector<Entity*>* entities);
  ShadowShader* setCamera(Camera* camera);
  static unsigned int getFboID();
  static unsigned int getDepthMap();
};
