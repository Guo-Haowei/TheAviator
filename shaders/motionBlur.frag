// motionBlur.frag
#version 330 core

uniform sampler2D colorTexture;
in vec2 pass_textureCoords;
out vec4 out_Color;

void main() {
  out_Color = texture(colorTexture, pass_textureCoords);
}
