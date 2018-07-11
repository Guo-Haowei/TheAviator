// sun.vert
#version 330 core
in vec2 position;
out vec2 textureCoords;
uniform vec4 transform;

void main() {
  textureCoords = position + vec2(0.5, 0.5);
  textureCoords.y = 1.0 - textureCoords.y;
  vec2 pos = position * transform.zw + transform.xy;
  gl_Position = vec4(pos, 0.0, 1.0);
}