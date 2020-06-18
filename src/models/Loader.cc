// Loader.cc
#include "Loader.h"
#include "glPrerequisites.h"
#include <iostream>
using std::vector;

vector<unsigned int> Loader::vaos;
vector<unsigned int> Loader::vbos;

RawModel* Loader::loadToVAO(vector<float>& data1, int data1Dimension, vector<float>& data2, int data2Dimension, vector<unsigned int>& indices) {
  unsigned int vaoID = createVAO();
  bindIndicesBuffer(indices);
  storeDataInAttributeList(0, data1Dimension, data1);
  storeDataInAttributeList(1, data2Dimension, data2);
  return new RawModel(vaoID, indices.size());
}

RawModel* Loader::loadToVAO(vector<float>& data1, int data1Dimension, vector<float>& data2, int data2Dimension) {
  unsigned int vaoID = createVAO();
  storeDataInAttributeList(0, data1Dimension, data1);
  storeDataInAttributeList(1, data2Dimension, data2);
  return new RawModel(vaoID, data1.size());
}

RawModel* Loader::loadToVAO(vector<float>& data, int dimension) {
  unsigned int vaoID = createVAO();
  storeDataInAttributeList(0, dimension, data);
  return new RawModel(vaoID, data.size(), 1);
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

void Loader::storeDataInAttributeList(unsigned int attrubuteNumber, int coordinateSize, vector<float>& data) {
  unsigned int vboID;
  glGenBuffers(1, &vboID);
  vbos.push_back(vboID);
  glBindBuffer(GL_ARRAY_BUFFER, vboID);
  glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data.front(), GL_STATIC_DRAW);
  glVertexAttribPointer(attrubuteNumber, coordinateSize, GL_FLOAT, GL_FALSE, 0, (void*) 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Loader::bindIndicesBuffer(vector<unsigned int>& indices) {
  unsigned int vboID;
  glGenBuffers(1, &vboID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices.front(), GL_STATIC_DRAW);
}
