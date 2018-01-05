// Renderer.cc
#include "Renderer.h"
#include "../entities/Entity.h"
#include "../entities/Camera.h"
#include <iostream>

Renderer::Renderer() {
  entityShader.setCamera(&primaryCamera)->setEntities(&allEntities);
}

Renderer::~Renderer() {
  backgroundShader.clean();
  entityShader.clean();
}

void Renderer::render() {
  backgroundShader.render();
  entityShader.render();
}
