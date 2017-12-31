// ShaderProgram.cc
#include "ShaderProgram.h"
#include "glPrerequisites.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using std::cout;

void ShaderProgram::init(const char*vertexFileName, const char* fragmentFileName, const char* geometryFileName) {
  programID = glCreateProgram();
  vertexShaderID = loadShader(vertexFileName, GL_VERTEX_SHADER);
  fragmentShaderID = loadShader(fragmentFileName, GL_FRAGMENT_SHADER);
  glAttachShader(programID, vertexShaderID);
  glAttachShader(programID, fragmentShaderID);
  if (geometryFileName != nullptr) {
    geometryShaderID = loadShader(geometryFileName, GL_GEOMETRY_SHADER);
    glAttachShader(programID, geometryShaderID);
  }
  bindAttributes();
  glLinkProgram(programID);

  int success;
  char infoLog[512];
  glGetProgramiv(programID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(programID, 512, NULL, infoLog);
    std::cout << "==================================================\n";
    std::cout << "ERROR::SHADER: Failed to link program\n\n" << infoLog;
    std::cout << "\n==================================================\n";
  }

  getAllUniformLocations();
}

void ShaderProgram::getAllUniformLocations() {
  location_transformationMatrix = getUniformLocation("transformationMatrix");
  location_lightSpaceMatrix = getUniformLocation("lightSpaceMatrix");
  location_ambientLightIntensity = getUniformLocation("ambientLightIntensity");
}

void ShaderProgram::start() {
  glUseProgram(programID);
}

void ShaderProgram::stop() {
  glUseProgram(0);
}

ShaderProgram::~ShaderProgram() {
  stop();
  glDetachShader(programID, vertexShaderID);
  glDeleteShader(vertexShaderID);
  if (geometryShaderID != 0) {
    glDetachShader(programID, geometryShaderID);
    glDeleteShader(geometryShaderID);
  }
  glDetachShader(programID, fragmentShaderID);
  glDeleteShader(fragmentShaderID);
  glDeleteProgram(programID);
}

void ShaderProgram::bindAttribute(unsigned int attribute, const char* variable) {
  glBindAttribLocation(programID, attribute, variable);
}

int ShaderProgram::getUniformLocation(const char* uniformName) {
  return glGetUniformLocation(programID, uniformName);
}

void ShaderProgram::loadInt(int location, int value) {
  glUniform1i(location, value);
}

void ShaderProgram::loadBool(int location, bool value) {
  glUniform1i(location, value ? 1 : 0);
}

void ShaderProgram::loadFloat(int location, float value) {
  glUniform1f(location, value);
}

void ShaderProgram::loadVector3f(int location, glm::vec3 vec) {
  glUniform3f(location, vec.x ,vec.y, vec.z);
}

void ShaderProgram::loadVector4f(int location, glm::vec4 vec) {
  glUniform4f(location, vec.x ,vec.y, vec.z, vec.w);
}

void ShaderProgram::loadMatrix4f(int location, glm::mat4 mat) {
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

unsigned int ShaderProgram::loadShader(const char* file, unsigned int type) {
  try {
    std::fstream fs(file);
    if (!fs.good()) {
      throw std::exception();
    }

    std::stringstream ss;
    ss << fs.rdbuf();
    std::string shaderSourceString = ss.str();
    const char* shaderSource = shaderSourceString.c_str();
    unsigned int shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, &shaderSource, NULL);
    glCompileShader(shaderID);

    int success;
    char infoLog[512];
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
      std::cout << "==================================================\n";
      std::cout << "ERROR::SHADER: Failed to compile shader\n\n" << infoLog;
      std::cout << "\n==================================================\n";
    }

    return shaderID;
  } catch (std::exception& e) {
    std::cout << "==================================================\n";
    std::cout << "ERROR::SHADER: Failed to read file\n";
    std::cout << e.what();
    std::cout << "\n==================================================\n";
    return 0;
  }
}
