// Renderer.h
#pragma once
#include <shaders/BackgroundShader.h>
#include <shaders/EntityShader.h>
#include <shaders/SeaShader.h>
#include <shaders/ShadowShader.h>
#include <shaders/UIShader.h>

class Renderer {
private:
  BackgroundShader backgroundShader;
  UIShader uiShader;
  EntityShader entityShader;
  SeaShader seaShader;
  ShadowShader seaShadowShader;
  ShadowShader entityShadowShader;

  unsigned int depthBuffer;

public:
  Renderer();
  ~Renderer();

  void render();
};
