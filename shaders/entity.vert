// entity.vert
#version 330 core
in vec3 position;
in vec3 normal;

out vec3 FragPos;
out vec3 Normal;
out vec3 ToCameraVector;

uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main() {
  vec4 worldPosition = transformationMatrix * vec4(position, 1.0);
  gl_Position = projectionMatrix * viewMatrix * worldPosition;

  FragPos = vec3(worldPosition);
  Normal = normalize(mat3(transpose(inverse(transformationMatrix))) * normal);
  ToCameraVector = (inverse(viewMatrix) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - worldPosition.xyz;
}
