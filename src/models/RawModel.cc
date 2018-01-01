// RawModel.cc
#include "RawModel.h"

RawModel::RawModel(unsigned int vaoID, unsigned int vertexCount): vaoID(vaoID), vertexCount(vertexCount) { }

unsigned int RawModel::getVaoID() const {
  return vaoID;
}

unsigned int RawModel::getVertexCount() const {
  return vertexCount;
}
