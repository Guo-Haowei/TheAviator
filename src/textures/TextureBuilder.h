#pragma once
#include "Texture.h"
#include <utils/File.h>

class TextureBuilder {
public:
  TextureBuilder(const File& file);
  Texture create();

  TextureBuilder* anisotropic();
  TextureBuilder* setFormat(int format);
  int getFormat() const;
  bool getClampEdges() const;
  bool getMipmap() const;
  bool getAnisotropic() const;
  bool getNearest() const;
private:
  File m_file;

  bool m_clampEdges;
  bool m_mipmap;
  bool m_anisotropic;
  bool m_nearest;
  int m_format;
};
