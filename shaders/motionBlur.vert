// motionBlur.vert
#version 330 core
in vec2 position;

out vec2 pass_textureCoords;

void main() {
  pass_textureCoords = position + vec2(0.5, 0.5);

  gl_Position = vec4(position * 2.0, 0.0, 1.0);
}