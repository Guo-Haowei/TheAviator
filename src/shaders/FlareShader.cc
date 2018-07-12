// FlareShader.cc
#include "FlareShader.h"
#include <common.h>
#include <textures/TextureBuilder.h>
#include <entities/gameObjects/Light.h>
#include <entities/gameObjects/Camera.h>
#include <models/Geometry.h>
#include <utils/Debug.h>
#include <GL/glew.h>
#include <string>
#include <iostream>
#include <math.h>
using std::cout;

FlareShader::FlareShader() {
  for (int i = 0; i < 9; ++i) {
    File textureFile("tex" + std::to_string(i+1), png);
    TextureBuilder builder(textureFile);
    m_textures[i] = builder.anisotropic()->create();
  }
  m_flareTextures[0].index = 5;
  m_flareTextures[0].scale = 0.5f;
  m_flareTextures[1].index = 3;
  m_flareTextures[1].scale = 0.23f;
  m_flareTextures[2].index = 1;
  m_flareTextures[2].scale = 0.1f;
  m_flareTextures[3].index = 6;
  m_flareTextures[3].scale = 0.05f;
  m_flareTextures[4].index = 0;
  m_flareTextures[4].scale = 0.02f;
  m_flareTextures[5].index = 2;
  m_flareTextures[5].scale = 0.06f;
  m_flareTextures[6].index = 8;
  m_flareTextures[6].scale = 0.12f;
  m_flareTextures[7].index = 4;
  m_flareTextures[7].scale = 0.07f;
  m_flareTextures[8].index = 0;
  m_flareTextures[8].scale = 0.012f;
  m_flareTextures[9].index = 6;
  m_flareTextures[9].scale = 0.2f;
  m_flareTextures[10].index = 8;
  m_flareTextures[10].scale = 0.1f;
  m_flareTextures[11].index = 2;
  m_flareTextures[11].scale = 0.07f;
  m_flareTextures[12].index = 4;
  m_flareTextures[12].scale = 0.3f;
  m_flareTextures[13].index = 3;
  m_flareTextures[13].scale = 0.4f;
  m_flareTextures[14].index = 7;
  m_flareTextures[14].scale = 0.6f;

  quad = Geometry::quad;
  const char* VERTEX_FILE = "../shaders/flare.vert";
  const char* FRAGMENT_FILE = "../shaders/flare.frag";
  ShaderProgram::init(VERTEX_FILE, FRAGMENT_FILE);
}

void FlareShader::bindAttributes() {
  bindAttribute(0, "position");
}

FlareShader::~FlareShader() {}

void FlareShader::getAllUniformLocations() {
  location_flareTexture = getUniformLocation("flareTexture");
  location_brightness = getUniformLocation("brightness");
  location_transform = getUniformLocation("transform");
}

void FlareShader::render() {
  glm::vec4 sunScreenPos = Camera::primary().getPVMatrix() * glm::vec4(Light::theOne().getPosition(), 1.0f);
  if (sunScreenPos.w < 0.0f)
    return;
  glm::vec2 sunScreenPos2d;
  sunScreenPos2d.x = (sunScreenPos.x / sunScreenPos.w + 1.0f) / 2.0f;
  sunScreenPos2d.y = 1.0f - ((sunScreenPos.y / sunScreenPos.w + 1.0f) / 2.0f);
  glm::vec2 sunToCenter = glm::vec2(0.5f, 0.5f) - sunScreenPos2d;
  float length = sqrt(sunToCenter.x * sunToCenter.x + sunToCenter.y * sunToCenter.y);
  float brightness = (1.0f - (length / 0.7f)) * 0.8f;
  if (brightness > 0.0f) {
    static float spacing = 0.13f;
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    start();
    loadFloat(location_brightness, brightness);
    quad->bind();
    for (int i = 0; i < TOTAL_FLARE_TEXTURE; ++i) {
      int index = m_flareTextures[i].index;
      float scaleX = m_flareTextures[i].scale;
      float scaleY = scaleX * (float)ACTUAL_WIDTH / (float)ACTUAL_HEIGHT;
      glm::vec2 flarePos = (float)i * spacing * sunToCenter + sunScreenPos2d;
      glm::vec4 transform(flarePos, scaleX, scaleY);
      loadVector4f(location_transform, transform);
      m_textures[index].bindToUint(0);
      glDrawArrays(GL_TRIANGLES, 0, quad->getVertexCount());
    }
    RawModel::unbind(1);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    stop();
  }
}
