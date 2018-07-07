#pragma once
#include <vector>

class File;
class Texture;
class TextureData;
class TextureBuilder;

namespace TextureUtils {
  TextureData decodeTextureFile(File& file);
  TextureData decodeTextureFile(File* file);
  unsigned int loadTextureToOpenGL(TextureData& data, TextureBuilder& builder);
  unsigned int createEmptyCubeMap(int size);
  unsigned int loadCubeMap(std::vector<File*>& files);
}
