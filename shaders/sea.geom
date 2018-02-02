// sea.geom
#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec4 viewSpace;
out vec3 Normal;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

vec3 getNormal() {
  vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
  vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
  return normalize(cross(a, b));
}

void main() {
  vec3 normal = getNormal();

  vec4 worldPosition = gl_in[0].gl_Position;
  viewSpace = viewMatrix * worldPosition;
  gl_Position = projectionMatrix * viewSpace;
  Normal = normal;
  EmitVertex();

  worldPosition = gl_in[1].gl_Position;
  viewSpace = viewMatrix * worldPosition;
  gl_Position = projectionMatrix * viewSpace;
  Normal = normal;
  EmitVertex();

  worldPosition = gl_in[2].gl_Position;
  viewSpace = viewMatrix * worldPosition;
  gl_Position = projectionMatrix * viewSpace;
  Normal = normal;
  EmitVertex();
  EndPrimitive();
}
