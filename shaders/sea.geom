// sea.geom
#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec4 ViewSpace;
out vec3 Normal;
out vec4 LightSpaceFragPos;
out vec3 FragPos;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 lightSpaceMatrix;

vec3 getNormal() {
  vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
  vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
  return normalize(cross(b, a));
}

void main() {
  vec3 normal = getNormal();

  vec4 worldPosition = gl_in[0].gl_Position;
  FragPos = vec3(worldPosition);
  LightSpaceFragPos = lightSpaceMatrix * worldPosition;
  ViewSpace = viewMatrix * worldPosition;
  gl_Position = projectionMatrix * ViewSpace;
  Normal = normal;
  EmitVertex();

  worldPosition = gl_in[1].gl_Position;
  FragPos = vec3(worldPosition);
  LightSpaceFragPos = lightSpaceMatrix * worldPosition;
  ViewSpace = viewMatrix * worldPosition;
  gl_Position = projectionMatrix * ViewSpace;
  Normal = normal;
  EmitVertex();

  worldPosition = gl_in[2].gl_Position;
  FragPos = vec3(worldPosition);
  LightSpaceFragPos = lightSpaceMatrix * worldPosition;
  ViewSpace = viewMatrix * worldPosition;
  gl_Position = projectionMatrix * ViewSpace;
  Normal = normal;
  EmitVertex();
  EndPrimitive();
}
