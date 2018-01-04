// Renderer.h
#pragma once
#include "../entities/Camera.h"
#include "../entities/Entity.h"
#include "../shaders/BackgroundShader.h"
#include "../shaders/EntityShader.h"
#include <vector>
using std::vector;

class Renderer {
private:
  BackgroundShader backgroundShader;
  EntityShader entityShader;
public:
  Renderer(Camera* camera, vector<Entity*>* entities);
  ~Renderer();

  void render();
};
