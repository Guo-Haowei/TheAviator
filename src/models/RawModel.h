// RawModel.h
#pragma once

class RawModel {
private:
  unsigned int vaoID;
  unsigned int vertexCount;
public:
  RawModel(unsigned int vaoID, unsigned int vertexCount);

  unsigned int getVaoID() const;
  unsigned int getVertexCount() const;

  void bind();
  static void unbind();
};
