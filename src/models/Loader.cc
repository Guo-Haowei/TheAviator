// Loader.cc
#define GLEW_STATIC
#include "Loader.h"
#include <GL/glew.h>
#include <iostream>
using std::vector;

vector<unsigned int> Loader::vaos;
vector<unsigned int> Loader::vbos;

RawModel* Loader::loadToVAO(vector<float> vertices, vector<float> normals, vector<float> colors) {
  unsigned int vaoID = createVAO();
  storeDataInAttributeList(0, 3, vertices);
  storeDataInAttributeList(1, 3, colors.size() == 0 ? normals : colors);
  return new RawModel(vaoID, vertices.size());
}

void Loader::clean() {
  for (int i = 0; i < vaos.size(); ++i) {
    glDeleteVertexArrays(1, &vaos[i]);
  }
  for (int i = 0; i < vaos.size(); ++i) {
    glDeleteBuffers(1, &vbos[i]);
  }
}

unsigned int Loader::createVAO() {
  unsigned int vaoID;
  glGenVertexArrays(1, &vaoID);
  vaos.push_back(vaoID);
  glBindVertexArray(vaoID);
  return vaoID;
}

void Loader::storeDataInAttributeList(unsigned int attrubuteNumber, int coordinateSize, vector<float> data) {
  unsigned int vboID;
  glGenBuffers(1, &vboID);
  vbos.push_back(vboID);
  glBindBuffer(GL_ARRAY_BUFFER, vboID);
  glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data.front(), GL_STATIC_DRAW);
  glVertexAttribPointer(attrubuteNumber, coordinateSize, GL_FLOAT, GL_FALSE, 0, (void*) 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  for (int i = 0 ; i < data.size(); ++i)
    std::cout << data[i] << std::endl;
}

void Loader::bindIndicesBuffer(vector<unsigned int> indices) {
  unsigned int vboID;
  glGenBuffers(1, &vboID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices.front(), GL_STATIC_DRAW);
}
