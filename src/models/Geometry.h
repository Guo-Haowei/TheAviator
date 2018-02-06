// Geometry.h
#pragma once
#include "RawModel.h"

namespace Geometry {
  // models
  extern RawModel* cube;
  extern RawModel* sea;
  extern RawModel* cockpit;

  void initGeometry();
  void cleanGeometry();
};

RawModel* createCube();
RawModel* createSea(float radius, float height, int radialSegments, int heightSegments);
RawModel* createCockpit();
