// Loader.h
#pragma once
#include "RawModel.h"
#include <vector>
using std::vector;

class Loader {
private:
  static vector<unsigned int> vaos;
  static vector<unsigned int> vbos;

  static unsigned int createVAO();
  static void storeDataInAttributeList(unsigned int attrubuteNumber, int coordinateSize, vector<float> data);
  static void bindIndicesBuffer(vector<unsigned int> indices);

public:
  static void clean();

  static RawModel* loadToVAO(vector<float> vertices, vector<float> normalsOrColors);
};

