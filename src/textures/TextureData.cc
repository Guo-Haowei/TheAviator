#include "TextureData.h"
#include "stb_image.h"

TextureData::TextureData(int width, int height, unsigned char* buffer):
  m_width(width), m_height(height), m_buffer(buffer) {}

TextureData& TextureData::operator=(TextureData& other) {
  m_width = other.m_width;
  m_height = other.m_height;
  m_buffer = other.m_buffer;
  other.m_buffer = nullptr;
  return *this;
}

TextureData::~TextureData() {
  if (m_buffer)
    stbi_image_free(m_buffer);
}

int TextureData::getWidth() const {
  return m_width;
}

int TextureData::getHeight() const {
  return m_height;
}

unsigned char* TextureData::getBuffer() const {
  return m_buffer;
}
