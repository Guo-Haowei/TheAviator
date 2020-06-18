// RawModel.cc
#include "RawModel.h"
#include "glPrerequisites.h"

RawModel::RawModel(unsigned int vaoID, unsigned int vertexCount, int vbos): vbos(vbos), vaoID(vaoID), vertexCount(vertexCount) { }

unsigned int RawModel::getVaoID() const {
  return vaoID;
}

unsigned int RawModel::getVertexCount() const {
  return vertexCount;
}

void RawModel::bind() {
  glBindVertexArray(vaoID);
  for (int i = 0; i < vbos; ++i) {
    glEnableVertexAttribArray(i);
  }
}

void RawModel::unbind(int vbos) {
  for (int i = 0; i < vbos; ++i) {
    glDisableVertexAttribArray(i);
  }
  glBindVertexArray(0);
}
