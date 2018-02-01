// Parser.cc
#include "Parser.h"
#include "../common.h"
#include <fstream>
#include <iostream>
#include <string>
using std::cout;
using std::ifstream;
using std::string;

float SEA::RADIUS;
float SEA::HEIGHT;
int SEA::RADIAL_SEGMENTS;
int SEA::HEIGHT_SEGMENTS;
float SEA::MIN_AMPLITUDE;
float SEA::MAX_AMPLITUDE;
float SEA::MIN_SPEED;
float SEA::MAX_SPEED;

float CAMERA::X;
float CAMERA::Y;
float CAMERA::Z;
float CAMERA::ZOOM;

float LIGHT::X;
float LIGHT::Y;
float LIGHT::Z;

// helper
void getNextFloat(ifstream& file, float* value) {
  string word;
  while (file >> word) {
    if ((word[0] >= '0' && word[0] <= '9') || word[0] == '-' || word[0] == '.') {
      *value = std::stof(word);
      return;
    }
  }
}

void getNextInt(ifstream& file, int* value) {
  string word;
  while (file >> word) {
    if ((word[0] >= '0' && word[0] <= '9') || word[0] == '-') {
      *value = std::stoi(word);
      return;
    }
  }
}

void Parser::parse() {
  ifstream configFile;
  configFile.open("../config.txt");
  getNextFloat(configFile, &SEA::RADIUS);
  getNextFloat(configFile, &SEA::HEIGHT);
  getNextInt(configFile, &SEA::RADIAL_SEGMENTS);
  getNextInt(configFile, &SEA::HEIGHT_SEGMENTS);
  getNextFloat(configFile, &SEA::MIN_AMPLITUDE);
  getNextFloat(configFile, &SEA::MAX_AMPLITUDE);
  getNextFloat(configFile, &SEA::MIN_SPEED);
  getNextFloat(configFile, &SEA::MAX_SPEED);

  getNextFloat(configFile, &CAMERA::X);
  getNextFloat(configFile, &CAMERA::Y);
  getNextFloat(configFile, &CAMERA::Z);
  getNextFloat(configFile, &CAMERA::ZOOM);

  getNextFloat(configFile, &LIGHT::X);
  getNextFloat(configFile, &LIGHT::Y);
  getNextFloat(configFile, &LIGHT::Z);
}
