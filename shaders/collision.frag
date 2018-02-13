// collision.frag
#version 330 core

out vec4 out_Color;

uniform float alpha;

void main() {
  out_Color = vec4(1.0, 1.0, 1.0, alpha);
}
