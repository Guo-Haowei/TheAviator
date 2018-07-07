#define STB_IMAGE_IMPLEMENTATION
#include "TextureUtils.h"
#include "Texture.h"
#include "TextureData.h"
#include "TextureBuilder.h"
#include "stb_image.h"
#include <utils/File.h>
#include <GL/glew.h>
#include <iostream>
using std::cout;
using std::vector;

unsigned int TextureUtils::createEmptyCubeMap(int size) {
  unsigned int textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
  for (int i = 0; i < 6; ++i) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, size, size,
      0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)0);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  return textureID;
}

unsigned int TextureUtils::loadCubeMap(vector<File*>& files) {
  unsigned int textureID;
  glGenTextures(1, &textureID);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  for (unsigned int i = 0; i < files.size(); ++i) {
    TextureData data = decodeTextureFile(files[i]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
      data.getWidth(), data.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
      data.getBuffer());
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  return textureID;
}

TextureData TextureUtils::decodeTextureFile(File& file) {
  int width, height, nrChannels;
  unsigned char *data = stbi_load(file.getPath().c_str(),
    &width, &height, &nrChannels, STBI_rgb_alpha);
  if (!data) {
    cout << "ERROR::TextureUtils: Failed to load texture ";
    cout << file.getPath() << "\n";
  }
  return TextureData(width, height, data);
}

TextureData TextureUtils::decodeTextureFile(File* file) {
  int width, height, nrChannels;
  unsigned char *data = stbi_load(file->getPath().c_str(),
    &width, &height, &nrChannels, STBI_rgb_alpha);
  if (!data) {
    cout << "ERROR::TextureUtils: Failed to load texture ";
    cout << file->getPath() << "\n";
  }
  return TextureData(width, height, data);
}

unsigned int TextureUtils::loadTextureToOpenGL(TextureData& data, TextureBuilder& builder) {
  unsigned int textureID;
  glGenTextures(1, &textureID);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, builder.getFormat(), data.getWidth(),
    data.getHeight(), 0, builder.getFormat(), GL_UNSIGNED_BYTE,
    data.getBuffer());

  // add options later
  if (builder.getMipmap()) {
    //glGenerateMipmap(GL_TEXTURE_2D);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  } else if (builder.getNearest()) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }

  if (builder.getClampEdges()) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  return textureID;
}
