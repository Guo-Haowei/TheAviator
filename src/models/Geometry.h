// Geometry.h
#pragma once
#include "RawModel.h"

namespace Geometry {
  // models
  extern RawModel* tetrahedron;
  extern RawModel* cube;
  extern RawModel* sphere;
  extern RawModel* sea;
  extern RawModel* cockpit;
  extern RawModel* propeller;

  void initGeometry();
  void cleanGeometry();
};
