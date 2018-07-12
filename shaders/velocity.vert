// veloctiy.vert
#version 330 core
in vec3 position;

uniform mat4 PVM;
uniform mat4 prevPVM;

smooth out vec4 pass_position;
smooth out vec4 pass_prevPosition;

void main() {
  vec4 position4 = vec4(position, 1.0);
  pass_position = PVM * position4;
  pass_prevPosition = prevPVM * position4;

  gl_Position = pass_position;
}
