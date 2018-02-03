// Renderer.h
#pragma once
#include "../shaders/BackgroundShader.h"
#include "../shaders/EntityShader.h"
#include "../shaders/SeaShader.h"
#include "../shaders/ShadowShader.h"
#include <vector>
using std::vector;

class Renderer {
private:
  BackgroundShader backgroundShader;
  EntityShader entityShader;
  SeaShader seaShader;
  ShadowShader seaShadowShader;
  ShadowShader entityShadowShader;
public:
  Renderer();
  ~Renderer();

  void render();
};
