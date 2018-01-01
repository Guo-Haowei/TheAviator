// ShaderProgram.h
#pragma once
#include <glm/glm.hpp>

class ShaderProgram {
private:
  unsigned int programID;
  unsigned int vertexShaderID;
  unsigned int fragmentShaderID;

  static unsigned int loadShader(const char* file, unsigned int type);
protected:
  virtual void getAllUniformLocations() = 0;
  virtual void bindAttributes() = 0;
  void bindAttribute(unsigned int attribute, const char* variable);
  int getUniformLocation(const char* uniformName);
  void loadFloat(int location, float value);
  void loadVector3f(int location, glm::vec3 vec);
  void loadMatrix4f(int location, glm::mat4 mat);
public:
  void init(const char* vertexFileName, const char* fragmentFileName);
  void start();
  void stop();
  void clean();
};

