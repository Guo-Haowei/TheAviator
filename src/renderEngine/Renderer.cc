// Renderer.cc
#include "Renderer.h"
#include <GL/glew.h>
#include <iostream>

Renderer::Renderer(Camera* camera, vector<Entity*>* entities) {
  entityShader.setCamera(camera)->setEntities(entities);
}

Renderer::~Renderer() {
  backgroundShader.clean();
  entityShader.clean();
}

void Renderer::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  backgroundShader.render();
  entityShader.render();
}
