// EntityShader.h
#pragma once
#include "ShaderProgram.h"

class EntityShader : public ShaderProgram
{
protected:
    int location_M;
    int location_V;
    int location_P;
    int location_light;
    int location_color;
    void bindAttributes();
    void getAllUniformLocations();

public:
    EntityShader();

    void render();
};
