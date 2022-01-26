// entity.frag
#version 330 core
in vec3 pass_position;
in vec3 pass_normal;

layout(location = 0) out vec4 colorTexture;

uniform vec3 color;
uniform vec3 lightPos;

void main() {
  vec3 unitNormal = normalize(pass_normal);

  // directional light
  vec3 lightDir = normalize(lightPos - pass_position);

  // ambient
  float ambientStrength = 0.3;
  vec3 ambient = ambientStrength * color;

  // diffuse
  float diff = max(dot(unitNormal, lightDir), 0.0);
  vec3 diffuse = diff * color;

  vec3 finalColor = ambient + diffuse;
  colorTexture = vec4(finalColor, 1.0);
}
