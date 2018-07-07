#pragma once

class TextureData {
public:
  TextureData(int width, int height, unsigned char* buffer);
  TextureData& operator=(TextureData& other);
  ~TextureData();

  int getWidth() const;
  int getHeight() const;
  unsigned char* getBuffer() const;
private:
  int m_width;
  int m_height;
  unsigned char* m_buffer;
};
