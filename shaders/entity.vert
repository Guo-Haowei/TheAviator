// entity.vert
#version 330 core
in vec3 in_position;
in vec3 in_normal;

out vec3 pass_position;
out vec3 pass_normal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main() {
  vec4 position4 = vec4(in_position, 1.0);
  vec4 worldPosition = M * position4;
  gl_Position = P * V * worldPosition;

  pass_normal = in_normal;
}
