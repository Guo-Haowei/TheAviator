// DBDepthMap.h
#if DB_DEPTHMAP
#pragma once
#include "ShaderProgram.h"

class RawModel;

class DBDepthMap: public ShaderProgram {
private:
  RawModel* texture;
protected:
  int location_shadowMap;
  int location_nearPlane;
  int location_farPlane;
  void bindAttributes();
  void getAllUniformLocations();
public:
  DBDepthMap();
  ~DBDepthMap();
  void render();  
};

#endif