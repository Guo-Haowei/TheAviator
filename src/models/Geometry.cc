// Geometry.cc
#include "Geometry.h"
#include "Loader.h"
#include <common.h>
#include <maths/Maths.h>
#include <utils/Debug.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <cassert>
using std::cout;
using std::vector;

RawModel* Geometry::cube;
RawModel* Geometry::sea;
RawModel* Geometry::sphere;
RawModel* Geometry::cockpit;
RawModel* Geometry::propeller;
RawModel* Geometry::tetrahedron;
RawModel* Geometry::quad;

RawModel* createTetrahedron(int segments = 1);
RawModel* createCube();
RawModel* createSea(float radius, float height, int radialSegments, int heightSegments);
RawModel* createCockpit();
RawModel* createPropeller();
RawModel* createQuad();

void Geometry::initGeometry() {
  tetrahedron = createTetrahedron();
  cube = createCube();
  sphere = createTetrahedron(4);
  sea = createSea(SEA::RADIUS, SEA::HEIGHT, SEA::RADIAL_SEGMENTS, SEA::HEIGHT_SEGMENTS);
  cockpit = createCockpit();
  propeller = createPropeller();
  quad = createQuad();
}

void Geometry::cleanGeometry() {
  delete tetrahedron;
  delete cube;
  delete sphere;
  delete sea;
  delete cockpit;
  delete propeller;
  delete quad;
}

/* helper functions for createTetrahedron */
#define TARGET_LENGTH 1.0

// want distance between each two vertices are the same
glm::vec3 normalizePoint(glm::vec3 inputPoint, float targetLength = TARGET_LENGTH, glm::vec3 targetPoint = glm::vec3(0.0f)) {
  glm::vec3 difference = inputPoint - targetPoint;
  difference = (targetLength / glm::length(difference)) * difference;
  return targetPoint + difference;
  float length = glm::length(inputPoint - targetPoint);
  return inputPoint * (targetLength / length);
}

// input three vertices in clock-wise order
void tetrahedronHelper(int segments, glm::vec3 vert1, glm::vec3 vert2, glm::vec3 vert3, vector<glm::vec3>* vertices) {
  glm::vec3 vStepSize = (vert3 - vert1) / (float)segments;
  glm::vec3 hStepSize = (vert2 - vert3) / (float)segments;
  for (int i = 0; i < segments; ++i) {
    // level i havs 2 * i + 1 triangles
    for (int j = 0; j < 2 * i + 1; ++j) {
      if (j % 2) { // j is odd, triangle is upside down
        glm::vec3 leftVert = vert1 + (float)i * vStepSize + (float)(j / 2) * hStepSize;
        glm::vec3 rightVert = leftVert + hStepSize;
        glm::vec3 bottomVert = rightVert + vStepSize;

        vertices->push_back(normalizePoint(leftVert));
        vertices->push_back(normalizePoint(rightVert));
        vertices->push_back(normalizePoint(bottomVert));
      } else { // j is even
        glm::vec3 topVert = vert1 + (float)i * vStepSize + (float)(j / 2) * hStepSize;
        glm::vec3 leftVert = topVert + vStepSize;
        glm::vec3 rightVert = leftVert + hStepSize;

        vertices->push_back(normalizePoint(topVert));
        vertices->push_back(normalizePoint(rightVert));
        vertices->push_back(normalizePoint(leftVert));
      }
    }
  }
}

RawModel* createTetrahedron(int segments) {
  assert(segments > 0);
  vector<glm::vec3> vertices;
  if (segments == 1) {
    glm::vec3 vert1(0.5f, 0.5f, 0.5f);
    glm::vec3 vert2(-0.5f, 0.5f, -0.5f);
    glm::vec3 vert3(-0.5f, -0.5f, 0.5f);
    glm::vec3 vert4(0.5f, -0.5f, -0.5f);
    tetrahedronHelper(segments, vert1, vert3, vert2, &vertices);
    tetrahedronHelper(segments, vert1, vert4, vert3, &vertices);
    tetrahedronHelper(segments, vert1, vert2, vert4, &vertices);
    tetrahedronHelper(segments, vert2, vert3, vert4, &vertices);
  } else {
    glm::vec3 vertTop(0.0f, 1.0f, 0.0f);
    glm::vec3 vertBot(0.0f, -1.0f, 0.0f);
    glm::vec3 vertA(1.0f, 0.0f, 1.0f);
    glm::vec3 vertB(-1.0f, 0.0f, 1.0f);
    glm::vec3 vertC(-1.0f, 0.0f, -1.0f);
    glm::vec3 vertD(1.0f, 0.0f, -1.0f);
    tetrahedronHelper(segments, vertTop, vertA, vertB, &vertices);
    tetrahedronHelper(segments, vertTop, vertB, vertC, &vertices);
    tetrahedronHelper(segments, vertTop, vertC, vertD, &vertices);
    tetrahedronHelper(segments, vertTop, vertD, vertA, &vertices);
    tetrahedronHelper(segments, vertBot, vertB, vertA, &vertices);
    tetrahedronHelper(segments, vertBot, vertC, vertB, &vertices);
    tetrahedronHelper(segments, vertBot, vertD, vertC, &vertices);
    tetrahedronHelper(segments, vertBot, vertA, vertD, &vertices);
  }

  vector<float> vertexArray, normals;
  for (int i = 0; i < vertices.size(); ++i) {
    vertexArray.push_back(vertices[i].x);
    vertexArray.push_back(vertices[i].y);
    vertexArray.push_back(vertices[i].z);
  }

  for (int i = 0; i < vertices.size(); i += 3) {
    glm::vec3 normal = glm::cross(vertices[i] - vertices[i+1], vertices[i+2] - vertices[i+1]);
    for (int j = 0; j < 3; ++j) {
      normals.push_back(normal.x);
      normals.push_back(normal.y);
      normals.push_back(normal.z);
    }
  }

  return Loader::loadToVAO(vertexArray, 3, normals, 3);
}

RawModel* createQuad() {
  vector<float> vertices;
  vertices.push_back(-0.5f);
  vertices.push_back(0.5f);
  vertices.push_back(0.5f);
  vertices.push_back(0.5f);
  vertices.push_back(0.5f);
  vertices.push_back(-0.5f);

  vertices.push_back(0.5f);
  vertices.push_back(-0.5f);
  vertices.push_back(-0.5f);
  vertices.push_back(-0.5f);
  vertices.push_back(-0.5f);
  vertices.push_back(0.5f);
  return Loader::loadToVAO(vertices, 2);
}

RawModel* createCube() {
  glm::vec3 vert0(0.5f, 0.5f, 0.5f);
  glm::vec3 vert1(0.5f, 0.5f, -0.5f);
  glm::vec3 vert2(0.5f, -0.5f, 0.5f);
  glm::vec3 vert3(0.5f, -0.5f, -0.5f);
  glm::vec3 vert4(-0.5f, 0.5f, -0.5f);
  glm::vec3 vert5(-0.5f, 0.5f, 0.5f);
  glm::vec3 vert6(-0.5f, -0.5f, -0.5f);
  glm::vec3 vert7(-0.5f, -0.5f, 0.5f);

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

RawModel* createSea(float radius, float height, int radialSegments, int heightSegments) {
  assert(radialSegments > 0);
  assert(heightSegments > 0);
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
