// velocity.frag
#version 330 core
smooth in vec4 pass_position;
smooth in vec4 pass_prevPosition;

out vec4 out_velocity;

void main() {
  vec2 a = (pass_position.xy / pass_position.w) * 0.5 + 0.5;
  vec2 b = (pass_prevPosition.xy / pass_prevPosition.w) * 0.5 + 0.5;
  vec2 diff = 50 * (a - b);
  out_velocity = vec4(diff, 0.1, 1.0);
}
