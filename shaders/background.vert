#version 330 core

in vec3 position;
in vec3 color;

out vec3 FragColor;

void main() {
  gl_Position = vec4(position, 1.0f);
  FragColor = color;
  if (position.x < .001 && position.y < .001 && position.z < .001)
    gl_Position = vec4(1.0f, 0.0f, -1.0f, 1.0f);
}
