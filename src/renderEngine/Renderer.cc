// Renderer.cc
#include "Renderer.h"
#include <iostream>

Renderer::Renderer(Camera* camera, vector<Entity*>* entities) {
  entityShader.setCamera(camera)->setEntities(entities);
}

Renderer::~Renderer() {
  backgroundShader.clean();
  entityShader.clean();
}

void Renderer::render() {
  backgroundShader.render();
  entityShader.render();
}
