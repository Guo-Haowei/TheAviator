#include "TextureBuilder.h"
#include "TextureData.h"
#include "TextureUtils.h"

TextureBuilder::TextureBuilder(const File& file):
  m_file(file), m_clampEdges(false), m_format(GL_RGBA),
  m_mipmap(false), m_anisotropic(true), m_nearest(false) {}

Texture TextureBuilder::create() {
  TextureData textureData = TextureUtils::decodeTextureFile(m_file);
  unsigned int textureID = TextureUtils::loadTextureToOpenGL(textureData, *this);
  return Texture(textureID, textureData.getWidth());
}

TextureBuilder* TextureBuilder::setFormat(int format) {
  m_format = format;
  return this;
}

TextureBuilder* TextureBuilder::anisotropic() {
  m_mipmap = true;
  m_anisotropic = true;
  return this;
}

int TextureBuilder::getFormat() const {
  return m_format;
}

bool TextureBuilder::getClampEdges() const {
  return m_clampEdges;
}

bool TextureBuilder::getMipmap() const {
  return m_mipmap;
}

bool TextureBuilder::getAnisotropic() const {
  return m_anisotropic;
}

bool TextureBuilder::getNearest() const {
  return m_nearest;
}
