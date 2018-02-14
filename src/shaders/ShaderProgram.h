// ShaderProgram.h
#pragma once
#include <glm/glm.hpp>

class ShaderProgram {
private:
  static unsigned int loadShader(const char* file, unsigned int type);
protected:
  unsigned int programID;
  unsigned int vertexShaderID;
  unsigned int fragmentShaderID;
  unsigned int geometryShaderID;

  int location_transformationMatrix;
  int location_lightSpaceMatrix;
  int location_ambientLightIntensity;

  virtual void getAllUniformLocations();
  virtual void bindAttributes() = 0;
  void bindAttribute(unsigned int attribute, const char* variable);
  int getUniformLocation(const char* uniformName);
  void loadInt(int location, int value);
  void loadBool(int location, bool value);
  void loadFloat(int location, float value);
  void loadVector3f(int location, glm::vec3 vec);
  void loadVector4f(int location, glm::vec4 vec);
  void loadMatrix4f(int location, glm::mat4 mat);
public:
  void init(const char* vertexFileName, const char* fragmentFileName, const char* geometryFileName = nullptr);
  void start();
  void stop();
  virtual ~ShaderProgram();
};

