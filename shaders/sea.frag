// sea.frag
#version 330 core
// in vec3 FragPos;
// in vec3 Normal;
// in vec3 ToCameraVector;

out vec4 out_Color;

void main() {
  vec3 seaColor = vec3(104.0f/255.0f, 195.0f/255.0f, 192.0f/255.0f);
  // vec3 unitNormal = normalize(Normal);
  // vec3 unitToCameraVector = normalize(ToCameraVector);

  // temp light properties
  // vec3 lightPos = vec3(0, 0, 5);
  // vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
  // vec3 lightDir = normalize(lightPos - FragPos);

  // attenuation
  // float dist = length(lightPos - FragPos);
  // float attenuation = 1.0 / (1.0 + 0.0014 * dist + 0.000007 * dist * dist);

  // ambient
  // float ambientStrength = 0.3;
  // vec3 ambient = ambientStrength * lightColor;
  // ambient *= attenuation;

  // diffuse
  // float diff = max(dot(unitNormal, lightDir), 0.0);
  // vec3 diffuse = diff * lightColor;
  // diffuse *= attenuation;

  // specular
  // float specularStrength = 0.5;
  // vec3 reflectedLightDir = reflect(-lightDir, unitNormal);
  // float specularFactor = pow(max(dot(reflectedLightDir, unitToCameraVector), 0.0), 64);
  // vec3 specular = specularStrength * specularFactor * lightColor;
  // specular *= attenuation;

  // out_Color = vec4((ambient + diffuse + specular) * color, 1.0f);
  out_Color = vec4(seaColor, 1.0f);
}
