#version 330 core
in vec2 TextureCoords;
out vec4 out_Color;

uniform sampler2D flareTexture;
uniform float brightness;

void main(void){
  out_Color = texture(flareTexture, TextureCoords);
  out_Color.a *= brightness;
}
