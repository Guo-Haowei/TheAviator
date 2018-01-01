#version 330 core

in vec3 position;
in vec3 color;

out vec3 FragColor;

void main() {
  gl_Position = vec4(position, 1.0f);
  FragColor = color;
}
