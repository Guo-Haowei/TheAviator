// Renderer.h
#pragma once
#include <shaders/BackgroundShader.h>
#include <shaders/EntityShader.h>

class Renderer
{
private:
    BackgroundShader backgroundShader;
    EntityShader entityShader;

    unsigned int frameBuffer;
    unsigned int colorTexture, velocityTexture;
    unsigned int depthBuffer;

public:
    Renderer();
    ~Renderer();

    void render();
};
