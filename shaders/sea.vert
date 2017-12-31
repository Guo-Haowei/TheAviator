// sea.vert
#version 330 core
in vec3 position;
in vec3 wave;

uniform mat4 transformationMatrix;
uniform float time;

void main() {
  // update wave position
  float angle = wave.x;
  float amplitude = wave.y;
  float speed = wave.z;
  float newX = position.x + cos(angle + time * speed) * amplitude;
  float newY = position.y + sin(angle + time * speed) * amplitude;
  gl_Position = transformationMatrix * vec4(newX, newY, position.z, 1.0);
}
