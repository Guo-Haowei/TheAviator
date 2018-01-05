// Renderer.h
#pragma once
#include "../shaders/BackgroundShader.h"
#include "../shaders/EntityShader.h"
#include <vector>
using std::vector;

class Renderer {
private:
  BackgroundShader backgroundShader;
  EntityShader entityShader;
public:
  Renderer();
  ~Renderer();

  void render();
};
