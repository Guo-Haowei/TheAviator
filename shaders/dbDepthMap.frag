// dbDepthMap.frag
#version 330 core

in vec2 TexCoord;
out vec4 out_Color;

uniform sampler2D depthMap;
uniform float nearPlane;
uniform float farPlane;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));  
}

void main() {
    float depthValue = texture(depthMap, TexCoord).r;
    out_Color = vec4(vec3(depthValue), 1.0);
}