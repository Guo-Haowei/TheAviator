// Renderer.cc
#include "Renderer.h"
#include <GL/glew.h>

Renderer::Renderer() { }

Renderer::~Renderer() {
}

void Renderer::prepare() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
