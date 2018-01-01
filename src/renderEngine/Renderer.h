// Renderer.h
#pragma once
#include "../shaders/BackgroundShader.h"

class Renderer {
private:
  BackgroundShader backgroundShader;
public:
  Renderer();
  ~Renderer();

  void prepare();
  void bindModel(RawModel* model);
  void unbindModel();
};
