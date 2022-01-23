#pragma once
#include <fstream>
#include <string>

#define SRC_PATH "../src/"
#define TEXTURE_PATH "../resources/textures/"

enum Extension {
  vert = 0,
  geom,
  frag,
  obj,
  png,
  txt,
  unknown
};

class File {
public:
  File();
  File(std::string name, Extension extension = unknown, std::string path = "");
  File(const File& other);

  void openFile(std::fstream& fs);
  bool getLine(std::string& line);
  Extension getExtension() const;
  std::string getPath() const;
private:
  Extension m_extension;
  std::string m_path;
  std::fstream m_fs;
};
