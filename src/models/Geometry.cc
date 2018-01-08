// Geometry.cc
#include "Geometry.h"
#include "Loader.h"
#include "../utils/Maths.h"
#include <glm/glm.hpp>
#include <math.h>
#include <vector>
#include <iostream>
using std::cout;
using std::vector;

RawModel* Geometry::cube;
RawModel* Geometry::sea;

void Geometry::initGeometry() {
  cube = createCube();
  sea = createSea(200, 300, 60, 10);
}

void Geometry::cleanGeometry() {
  delete cube;
  delete sea;
}

RawModel* createCube() {
  float normalsData[] = {
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, 1.0f,
    -1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, 1.0f, 0.0f
  };

  float verticesData[]  = {
    -0.1f, -0.1f, -0.1f,
    0.1f, -0.1f, -0.1f,
    0.1f,  0.1f, -0.1f,
    0.1f,  0.1f, -0.1f,
    -0.1f,  0.1f, -0.1f,
    -0.1f, -0.1f, -0.1f,

    -0.1f, -0.1f,  0.1f,
    0.1f, -0.1f,  0.1f,
    0.1f,  0.1f,  0.1f,
    0.1f,  0.1f,  0.1f,
    -0.1f,  0.1f,  0.1f,
    -0.1f, -0.1f,  0.1f,

    -0.1f,  0.1f,  0.1f,
    -0.1f,  0.1f, -0.1f,
    -0.1f, -0.1f, -0.1f,
    -0.1f, -0.1f, -0.1f,
    -0.1f, -0.1f,  0.1f,
    -0.1f,  0.1f,  0.1f,

    0.1f,  0.1f,  0.1f,
    0.1f,  0.1f, -0.1f,
    0.1f, -0.1f, -0.1f,
    0.1f, -0.1f, -0.1f,
    0.1f, -0.1f,  0.1f,
    0.1f,  0.1f,  0.1f,

    -0.1f, -0.1f, -0.1f,
    0.1f, -0.1f, -0.1f,
    0.1f, -0.1f,  0.1f,
    0.1f, -0.1f,  0.1f,
    -0.1f, -0.1f,  0.1f,
    -0.1f, -0.1f, -0.1f,

    -0.1f,  0.1f, -0.1f,
    0.1f,  0.1f, -0.1f,
    0.1f,  0.1f,  0.1f,
    0.1f,  0.1f,  0.1f,
    -0.1f,  0.1f,  0.1f,
    -0.1f,  0.1f, -0.1f
  };

  vector<float> vertices(verticesData, verticesData + sizeof(verticesData) / sizeof(float)), normals;
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      normals.push_back(normalsData[i * 3]);
      normals.push_back(normalsData[i * 3 + 1]);
      normals.push_back(normalsData[i * 3 + 2]);
    }
  }

  return Loader::loadToVAO(vertices, 3, normals, 3);
}

RawModel* createSea(float radius, float height, int radialSegments, int heightSegments) {
  float angleIncrement = 360 / radialSegments;
  float gridHeight = height / heightSegments;
  vector<float> vertices, waves;
  vector<unsigned int> indices;
  for (int i = 0; i <= heightSegments; ++i) {
    for (float angle = 0; angle < 360; angle += angleIncrement) {
      // vertices
      vertices.push_back(sin(angle * PI / 180.0f) * radius);
      float y = (i - heightSegments / 2) * gridHeight;
      vertices.push_back(y);
      vertices.push_back(cos(angle * PI / 180.0f) * radius);

      waves.push_back(Maths::rand(0.0f, 2 * PI)); // angle
      waves.push_back(Maths::rand(3.0f, 7.0f)); // amplitude
      waves.push_back(Maths::rand(0.016f, 0.048f)); // speed
    }
  }


  int index1 = radialSegments - 1;
  for (int i = 0; i < heightSegments; ++i) {
    for (int index2 = 0; index2 < radialSegments; index1 = index2++) {
      int point1 = index1 + i * radialSegments, point2 = index2 + i * radialSegments, point3 = point1 + radialSegments, point4 = point2 + radialSegments;
      indices.push_back(point1);
      indices.push_back(point2);
      indices.push_back(point3);
      indices.push_back(point2);
      indices.push_back(point4);
      indices.push_back(point3);
    }
  }

  return Loader::loadToVAO(vertices, 3, waves, 3, indices);
}
