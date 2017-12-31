#include "Texture.h"
#include <iostream>
using std::cout;

Texture::Texture(): m_textureID(0), m_size(0), m_type(GL_TEXTURE_2D) {}

Texture::Texture(const Texture& other):
  m_textureID(other.m_textureID), m_size(other.m_size), m_type(other.m_type) {}

Texture::Texture(unsigned int textureID, int size, int type):
  m_textureID(textureID), m_size(size), m_type(type) {}

void Texture::bindToUint(unsigned int unit) {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(m_type, m_textureID);
}

unsigned int Texture::getID() const {
  return m_textureID;
}

int Texture::getSize() const {
  return m_size;
}

void Texture::setTextureID(unsigned int textureID) {
  m_textureID = textureID;
}
