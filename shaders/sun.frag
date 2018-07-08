// sun.frag
#version 330 core
out vec4 out_Color;
in vec2 textureCoords;
uniform sampler2D sunTexture;

void main() {
  vec4 color = texture(sunTexture, textureCoords);
  out_Color = vec4(1, 0, 0, 1);
}