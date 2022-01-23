#include "File.h"
#include <iostream>
using std::string;
using std::cout;

string extensionTable[] = {".vert", ".geom", ".frag", ".obj", ".png", ".txt", "" };
string baseDir(Extension extension);

File::File(): m_path(""), m_extension(unknown) {}

File::File(string name, Extension extension, string path):
  m_extension(extension) {
  m_path = baseDir(extension) + path + "/" + name + extensionTable[extension];
}

File::File(const File& other):
  m_extension(other.m_extension), m_path(other.m_path) {}

void File::openFile(std::fstream& fs) {
  fs.open(m_path);
  if (!fs.is_open())
    cout << "ERROR::File: Failed to open file " << m_path << "\n";
}

bool File::getLine(string& line) {
  if (!m_fs.is_open()) {
    m_fs.open(m_path);
    if (!m_fs.is_open()) {
      cout << "ERROR::File: Failed to open file " << m_path << "\n";
      return false;
    }
  }
  while (getline(m_fs, line)) {
    if (!line.length())
      continue;
    if (line[0] == '#')
      continue;
    return true;
  }
  return false;
}

string baseDir(Extension extension) {
  switch (extension) {
    case vert:
    case geom:
    case frag:
      return SRC_PATH;
    case txt:
    case obj:
    case png:
      return TEXTURE_PATH;
    default:
      return "";
  }
}

Extension File::getExtension() const {
  return m_extension;
}

std::string File::getPath() const {
  return m_path;
}
