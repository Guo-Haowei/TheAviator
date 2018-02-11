// DBDepthMap.cc
#if DB_DEPTHMAP
#include "DBDepthMap.h"
#include <common.h>
#include <models/RawModel.h>
#include <models/Loader.h>
#include <GL/glew.h>
#include <vector>
using std::vector;

DBDepthMap::DBDepthMap() {
  float verticesData[] = {
    // top left
    -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,

    // bottom right
    1.0f,  1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f
  };
  float texCoordsData[] = {
    0.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,

    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f
  };
  vector<float> vertices(verticesData, verticesData + sizeof(verticesData) / sizeof(float));
  vector<float> texCoords(texCoordsData, texCoordsData + sizeof(texCoordsData) / sizeof(float));
  texture = Loader::loadToVAO(vertices, 3, texCoords, 2);

  const char* VERTEX_FILE = "../shaders/dbDepthMap.vert";
  const char* FRAGMENT_FILE = "../shaders/dbDepthMap.frag";
  init(VERTEX_FILE, FRAGMENT_FILE);
}

DBDepthMap::~DBDepthMap() {
  delete texture;
}

void DBDepthMap::bindAttributes() {
  bindAttribute(0, "position");
  bindAttribute(1, "texCoord");
}

void DBDepthMap::getAllUniformLocations() {
  location_shadowMap = getUniformLocation("shadowMap");
  location_nearPlane = getUniformLocation("nearPlane");
  location_farPlane = getUniformLocation("farPlane");
}

void DBDepthMap::render() {
  start();
  loadInt(location_shadowMap, 0);
  loadFloat(location_nearPlane, SHADOW::NEAR_PLANE);
  loadFloat(location_farPlane, SHADOW::FAR_PLANE);
  texture->bind();
  glDrawArrays(GL_TRIANGLES, 0, texture->getVertexCount());
  RawModel::unbind();
  stop();
}

#endif