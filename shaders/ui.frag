// ui.frag
#version 330 core
out vec4 out_Color;

uniform float health;
uniform float width;
uniform float height;

void main() {
  float x = gl_FragCoord.x;
  float y = gl_FragCoord.y;
  float size = height / 40;
  float padding = size / 5;
  float interval = size + padding;
  float yOffset = height - 2 * size;
  float xOffset = 2 * size;
  float length = (health / 5) * interval;
  if (y >= yOffset || y < yOffset - size || x < xOffset || x > xOffset + length) {
    discard;
  }
  x -= xOffset;
  x -= floor(x / interval) * interval;
  if (x > size) {
    discard;
  }
  if (health < 30.0) {
    out_Color = vec4(242.0f/255.0f, 53.0f/255.0f, 30.0f/255.0f, 0.8f);
  } else {
    out_Color = vec4(104.0f/255.0f, 195.0f/255.0f, 192.0f/255.0f, 0.8f);
  }
}