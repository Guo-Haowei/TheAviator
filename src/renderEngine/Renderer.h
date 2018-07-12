// Renderer.h
#pragma once
#include <shaders/BackgroundShader.h>
#include <shaders/FlareShader.h>
#include <shaders/UIShader.h>
#include <shaders/SunShader.h>
#include <shaders/EntityShader.h>
#include <shaders/SeaShader.h>
#include <shaders/ShadowShader.h>
#include <shaders/VelocityShader.h>
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
  SunShader sunShader;
  FlareShader flareShader;
  VelocityShader velocityShader;
public:
  Renderer();
  ~Renderer();

  void render();
};
