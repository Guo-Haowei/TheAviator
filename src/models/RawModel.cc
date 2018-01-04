// RawModel.cc
#include "RawModel.h"
#include <GL/glew.h>

RawModel::RawModel(unsigned int vaoID, unsigned int vertexCount): vaoID(vaoID), vertexCount(vertexCount) { }

unsigned int RawModel::getVaoID() const {
  return vaoID;
}

unsigned int RawModel::getVertexCount() const {
  return vertexCount;
}

void RawModel::bind() {
  glBindVertexArray(vaoID);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
}

void RawModel::unbind() {
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glBindVertexArray(0);
}
