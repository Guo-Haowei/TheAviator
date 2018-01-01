// Renderer.cc
#include "Renderer.h"
#include <GL/glew.h>
#include <iostream>

Renderer::Renderer() { }

Renderer::~Renderer() {
  backgroundShader.clean();
}

void Renderer::prepare() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // gradient background
  backgroundShader.start();
  bindModel(backgroundShader.getModel());
  glDrawArrays(GL_TRIANGLES, 0, backgroundShader.getModel()->getVertexCount());
  unbindModel();
  backgroundShader.stop();
}

void Renderer::bindModel(RawModel* model) {
  glBindVertexArray(model->getVaoID());
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
}

void Renderer::unbindModel() {
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glBindVertexArray(0);
}
