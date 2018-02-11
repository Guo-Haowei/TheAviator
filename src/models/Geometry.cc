// Geometry.cc
#include "Geometry.h"
#include "Loader.h"
#include "../common.h"
#include "../utils/Maths.h"
#include <glm/glm.hpp>
#include <math.h>
#include <vector>
#include <iostream>
using std::cout;
using std::vector;

RawModel* Geometry::cube;
RawModel* Geometry::sea;
RawModel* Geometry::cockpit;
RawModel* Geometry::propeller;
RawModel* Geometry::tetrahedron;

void Geometry::initGeometry() {
  tetrahedron = createTetrahedron();
  cube = createCube();
  sea = createSea(SEA::RADIUS, SEA::HEIGHT, SEA::RADIAL_SEGMENTS, SEA::HEIGHT_SEGMENTS);
  cockpit = createCockpit();
  propeller = createPropeller();
}

void Geometry::cleanGeometry() {
  delete tetrahedron;
  delete cube;
  delete sea;
  delete cockpit;
  delete propeller;
}

RawModel* createTetrahedron() {
  glm::vec3 vert0(0.5f, 0.5f, 0.5f);
  glm::vec3 vert1(-0.5f, 0.5f, -0.5f);
  glm::vec3 vert2(-0.5f, -0.5f, 0.5f);
  glm::vec3 vert3(0.5f, -0.5f, -0.5f);

  glm::vec3 vertices[12];
  // face 1
  vertices[0] = vert0;
  vertices[1] = vert2;
  vertices[2] = vert1;
  // face 2
  vertices[3] = vert0;
  vertices[4] = vert1;
  vertices[5] = vert3;
  // face 3
  vertices[6] = vert2;
  vertices[7] = vert3;
  vertices[8] = vert1;
  // face 3
  vertices[9] = vert0;
  vertices[10] = vert3;
  vertices[11] = vert2;

  vector<float> vertexArray, normals;
  for (int i = 0; i < 12; ++i) {
    vertexArray.push_back(vertices[i].x);
    vertexArray.push_back(vertices[i].y);
    vertexArray.push_back(vertices[i].z);
  }

  for (int i = 0; i < 4; ++i) {
    glm::vec3 vertA = vertices[4 * i];
    glm::vec3 vertB = vertices[4 * i + 1];
    glm::vec3 vertC = vertices[4 * i + 2];
    glm::vec3 normal = glm::cross(vertA - vertB, vertC - vertB);
    for (int j = 0; j < 3; ++j) {
      normals.push_back(normal.x);
      normals.push_back(normal.y);
      normals.push_back(normal.z);
    }
  }

  return Loader::loadToVAO(vertexArray, 3, normals, 3);
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
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
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

      waves.push_back(Maths::rand(0.0f, 2 * PI));
      waves.push_back(Maths::rand(SEA::MIN_AMPLITUDE, SEA::MAX_AMPLITUDE));
      waves.push_back(Maths::rand(SEA::MIN_SPEED, SEA::MAX_SPEED));
    }
  }

  // two circles
  vertices.push_back(0.0f);
  vertices.push_back(-heightSegments/2);
  vertices.push_back(0.0f);
  waves.push_back(Maths::rand(0.0f, 2 * PI));
  waves.push_back(Maths::rand(SEA::MIN_AMPLITUDE, SEA::MAX_AMPLITUDE));
  waves.push_back(Maths::rand(SEA::MIN_SPEED, SEA::MAX_SPEED));


  int index1 = radialSegments - 1;
  for (int i = 0; i < heightSegments; ++i) {
    for (int index2 = 0; index2 < radialSegments; index1 = index2++) {
      int point1 = index1 + i * radialSegments, point2 = index2 + i * radialSegments, point3 = point1 + radialSegments, point4 = point2 + radialSegments;
      // triangle 1
      indices.push_back(point2);
      indices.push_back(point1);
      indices.push_back(point3);
      // triangle 2
      indices.push_back(point2);
      indices.push_back(point3);
      indices.push_back(point4);
    }
  }

  int topPoint = vertices.size() - 1;
  index1 = radialSegments - 1;
  for (int i = 0; i < radialSegments - 1; index1 = i++) {
    indices.push_back(topPoint);
    indices.push_back(i);
    indices.push_back(index1);
  }

  return Loader::loadToVAO(vertices, 3, waves, 3, indices);
}

RawModel* createCockpit() {
  glm::vec3 vert0(4, 2.5, 2.5);
  glm::vec3 vert1(4, 2.5, -2.5);
  glm::vec3 vert2(4, -2.5, 2.5);
  glm::vec3 vert3(4, -2.5, -2.5);
  glm::vec3 vert4(-4, 1.5, -0.5);
  glm::vec3 vert5(-4, 1.5, 0.5);
  glm::vec3 vert6(-4, 0.5, -0.5);
  glm::vec3 vert7(-4, 0.5, 0.5);

  glm::vec3 vertices[36];
  // face left
  vertices[0] = vert0;
  vertices[1] = vert1;
  vertices[2] = vert3;
  vertices[3] = vert0;
  vertices[4] = vert3;
  vertices[5] = vert2;
  // face right
  vertices[6] = vert4;
  vertices[7] = vert5;
  vertices[8] = vert6;
  vertices[9] = vert5;
  vertices[10] = vert7;
  vertices[11] = vert6;
  // face front
  vertices[12] = vert0;
  vertices[13] = vert2;
  vertices[14] = vert7;
  vertices[15] = vert0;
  vertices[16] = vert7;
  vertices[17] = vert5;
  // face back
  vertices[18] = vert1;
  vertices[19] = vert6;
  vertices[20] = vert3;
  vertices[21] = vert1;
  vertices[22] = vert4;
  vertices[23] = vert6;
  // face up
  vertices[24] = vert4;
  vertices[25] = vert1;
  vertices[26] = vert0;
  vertices[27] = vert4;
  vertices[28] = vert0;
  vertices[29] = vert5;
  // face down
  vertices[30] = vert6;
  vertices[31] = vert2;
  vertices[32] = vert3;
  vertices[33] = vert6;
  vertices[34] = vert7;
  vertices[35] = vert2;

  vector<float> vertexArray, normals;
  for (int i = 0; i < 36; ++i) {
    vertexArray.push_back(vertices[i].x);
    vertexArray.push_back(vertices[i].y);
    vertexArray.push_back(vertices[i].z);
  }

  for (int i = 0; i < 6; ++i) {
    glm::vec3 point0 = vertices[i * 6];
    glm::vec3 point1 = vertices[i * 6 + 1];
    glm::vec3 point2 = vertices[i * 6 + 2];
    glm::vec3 normal = glm::cross(point0 - point1, point2 - point1);
    for (int j = 0; j < 6; ++j) {
      normals.push_back(normal.x);
      normals.push_back(normal.y);
      normals.push_back(normal.z);
    }
  }

  return Loader::loadToVAO(vertexArray, 3, normals, 3);
}

RawModel* createPropeller() {
  glm::vec3 vert0(0.5, 0.5, 0.5);
  glm::vec3 vert1(0.5, 0.5, -0.5);
  glm::vec3 vert2(0.5, -0.5, 0.5);
  glm::vec3 vert3(0.5, -0.5, -0.5);
  glm::vec3 vert4(-0.5, 0, 0);
  glm::vec3 vert5(-0.5, 0, 0);
  glm::vec3 vert6(-0.5, 0, 0);
  glm::vec3 vert7(-0.5, 0, 0);

  glm::vec3 vertices[36];
  // face left
  vertices[0] = vert0;
  vertices[1] = vert1;
  vertices[2] = vert3;
  vertices[3] = vert0;
  vertices[4] = vert3;
  vertices[5] = vert2;
  // face right
  vertices[6] = vert4;
  vertices[7] = vert5;
  vertices[8] = vert6;
  vertices[9] = vert5;
  vertices[10] = vert7;
  vertices[11] = vert6;
  // face front
  vertices[12] = vert0;
  vertices[13] = vert2;
  vertices[14] = vert7;
  vertices[15] = vert0;
  vertices[16] = vert7;
  vertices[17] = vert5;
  // face back
  vertices[18] = vert1;
  vertices[19] = vert6;
  vertices[20] = vert3;
  vertices[21] = vert1;
  vertices[22] = vert4;
  vertices[23] = vert6;
  // face up
  vertices[24] = vert4;
  vertices[25] = vert1;
  vertices[26] = vert0;
  vertices[27] = vert4;
  vertices[28] = vert0;
  vertices[29] = vert5;
  // face down
  vertices[30] = vert6;
  vertices[31] = vert2;
  vertices[32] = vert3;
  vertices[33] = vert6;
  vertices[34] = vert7;
  vertices[35] = vert2;

  vector<float> vertexArray, normals;
  for (int i = 0; i < 36; ++i) {
    vertexArray.push_back(vertices[i].x);
    vertexArray.push_back(vertices[i].y);
    vertexArray.push_back(vertices[i].z);
  }

  for (int i = 0; i < 6; ++i) {
    glm::vec3 point0 = vertices[i * 6];
    glm::vec3 point1 = vertices[i * 6 + 1];
    glm::vec3 point2 = vertices[i * 6 + 2];
    glm::vec3 normal = glm::cross(point0 - point1, point2 - point1);
    for (int j = 0; j < 6; ++j) {
      normals.push_back(normal.x);
      normals.push_back(normal.y);
      normals.push_back(normal.z);
    }
  }

  return Loader::loadToVAO(vertexArray, 3, normals, 3);
}
