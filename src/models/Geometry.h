// Geometry.h
#pragma once
#include "RawModel.h"
#include <glm/glm.hpp>
#include <string>

namespace Geometry {
  // models
  extern RawModel* tetrahedron;
  extern RawModel* cube;
  extern RawModel* sphere;
  extern RawModel* sea;
  extern RawModel* cockpit;
  extern RawModel* propeller;
  extern RawModel* quad;

  void initGeometry();
  void cleanGeometry();
};
