// RawModel.h
#pragma once

class RawModel {
private:
  unsigned int vaoID;
  unsigned int vertexCount;
  int vbos;
public:
  RawModel(unsigned int vaoID, unsigned int vertexCount, int vbos = 2);

  unsigned int getVaoID() const;
  unsigned int getVertexCount() const;

  void bind();
  static void unbind(int vbos = 2);
};
