// entity.frag
#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 ToCameraVector;

out vec4 out_Color;

uniform vec3 color;
uniform vec3 lightPos;
uniform sampler2D shadowMap;

void main() {
  vec3 unitNormal = normalize(Normal);
  vec3 unitToCameraVector = normalize(ToCameraVector);

  // directional light
  vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
  vec3 lightDir = normalize(lightPos);

  // ambient
  float ambientStrength = 0.3;
  vec3 ambient = ambientStrength * lightColor;

  // diffuse
  float diff = max(dot(unitNormal, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;

  // specular
  float specularStrength = 0.5;
  vec3 reflectedLightDir = reflect(-lightDir, unitNormal);
  float specularFactor = pow(max(dot(reflectedLightDir, unitToCameraVector), 0.0), 64);
  vec3 specular = specularStrength * specularFactor * lightColor;

  out_Color = vec4((ambient + diffuse + specular) * color, 1.0f);
}
