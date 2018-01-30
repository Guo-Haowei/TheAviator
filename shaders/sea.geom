// sea.geom
#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

vec3 getNormal() {
  vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
  vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
  return normalize(cross(a, b));
}

void main() {
  vec3 normal = getNormal();
  mat4 projAndView = projectionMatrix * viewMatrix;

  vec4 worldPosition = gl_in[0].gl_Position;
  gl_Position = projAndView * worldPosition;
  Normal = normal;
  EmitVertex();

  worldPosition = gl_in[1].gl_Position;
  gl_Position = projAndView * worldPosition;
  Normal = normal;
  EmitVertex();

  worldPosition = gl_in[2].gl_Position;
  gl_Position = projAndView * worldPosition;
  Normal = normal;
  EmitVertex();
  EndPrimitive();
}
