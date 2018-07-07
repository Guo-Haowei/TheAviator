// sun.frag
#version 330 core
out vec4 out_Color;
in vec2 textureCoords;
uniform sampler2D sunTexture;

void main() {
  out_Color = texture(sunTexture, textureCoords);
  // out_Color = vec4(0, 0, 1, 1);
}