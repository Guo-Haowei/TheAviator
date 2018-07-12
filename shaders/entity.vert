// entity.vert
#version 330 core
in vec3 position;
in vec3 normal;

out vec3 FragPos;
out vec3 Normal;
out vec3 ToCameraVector;
out vec4 LightSpaceFragPos;
out vec4 ViewSpace;
smooth out vec4 CurPos;
smooth out vec4 PrevPos;

uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 lightSpaceMatrix;
uniform mat4 prevPVM;

void main() {
  vec4 position4 = vec4(position, 1.0);
  vec4 worldPosition = transformationMatrix * position4;
  ViewSpace = viewMatrix * worldPosition;
  PrevPos = prevPVM * position4;
  CurPos = projectionMatrix * ViewSpace;
  gl_Position = CurPos;

  FragPos = vec3(worldPosition);
  Normal = normalize(mat3(transpose(inverse(transformationMatrix))) * normal);
  ToCameraVector = (inverse(viewMatrix) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - worldPosition.xyz;
  LightSpaceFragPos = lightSpaceMatrix * worldPosition;
}
