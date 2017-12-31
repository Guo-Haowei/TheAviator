#pragma once
#include "glPrerequisites.h"
#include <vector>

class File;

class Texture {
public:
  Texture();
  Texture(const Texture& other);
  Texture(unsigned int textureID, int size, int type = GL_TEXTURE_2D);

  void bindToUint(unsigned int unit);

  unsigned int getID() const;
  int getSize() const;
  void setTextureID(unsigned int textureID);
private:
  unsigned int m_textureID;
  int m_size;
  int m_type;
};
