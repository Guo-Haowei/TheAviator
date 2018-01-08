// sea.vert
#version 330 core
in vec3 position;
in vec3 wave;

// out vec3 FragPos;
// out vec3 Normal;
// out vec3 ToCameraVector;

uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform float time;

void main() {
  // update wave position
  float angle = wave.x;
  float amplitude = wave.y;
  float speed = wave.z;
  float newX = position.x + cos(angle + time * speed) * amplitude;
  float newY = position.y + sin(angle + time * speed) * amplitude;
  vec4 worldPosition = transformationMatrix * vec4(newX, newY, position.z, 1.0);
  gl_Position = projectionMatrix * viewMatrix * worldPosition;

  // FragPos = vec3(worldPosition);
  // Normal = normalize(mat3(transpose(inverse(transformationMatrix))) * normal);
  // ToCameraVector = (inverse(viewMatrix) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - worldPosition.xyz;
}
