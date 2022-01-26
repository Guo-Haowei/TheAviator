// Renderer.h
#pragma once
#include <shaders/EntityShader.h>

class Renderer
{
private:
    EntityShader entityShader;

    unsigned int frameBuffer;
    unsigned int colorTexture, velocityTexture;
    unsigned int depthBuffer;

public:
    Renderer();
    ~Renderer();

    void render();
};
