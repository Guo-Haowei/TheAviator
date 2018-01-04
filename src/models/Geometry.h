// Geometry.h
#pragma once
#include "RawModel.h"

namespace Geometry {
  // models
  extern RawModel* cube;

  void initGeometry();
  void cleanGeometry();
};

RawModel* createCube();
