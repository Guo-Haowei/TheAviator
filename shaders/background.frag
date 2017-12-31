// background.frag
#version 330 core
in vec3 FragColor;

out vec4 out_Color;

void main() {
  out_Color = vec4(FragColor, 1.0f);
}
