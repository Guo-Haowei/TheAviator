// collision.frag
#version 330 core

out vec4 out_Color;

uniform vec3 color;

void main() {
  out_Color = vec4(color, 1.0);
}
