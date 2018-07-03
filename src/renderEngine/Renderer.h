// Renderer.h
#pragma once
#include <shaders/BackgroundShader.h>
#include <shaders/UIShader.h>
#include <shaders/EntityShader.h>
#include <shaders/SeaShader.h>
#include <shaders/ShadowShader.h>
#include <shaders/DBDepthMap.h>
#include <vector>
using std::vector;

class Renderer {
private:
  BackgroundShader backgroundShader;
  UIShader uiShader;
  EntityShader entityShader;
  SeaShader seaShader;
  ShadowShader seaShadowShader;
  ShadowShader entityShadowShader;
#if DB_DEPTHMAP
  DBDepthMap dbDepthMapSahder;
#endif
public:
  Renderer();
  ~Renderer();

  void render();
};
