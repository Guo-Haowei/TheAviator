// sea.frag
#version 330 core
in vec4 ViewSpace;
in vec3 Normal;
in vec4 LightSpaceFragPos;

out vec4 out_Color;

uniform vec3 lightPos;
uniform sampler2D shadowMap;

float shadowCalculation(vec4 lightSpaceFragPos) {
  vec3 projCoords = lightSpaceFragPos.xyz / lightSpaceFragPos.w;
  projCoords = projCoords * 0.5 + 0.5;
  float closestDepth = texture(shadowMap, projCoords.xy).r;
  float currentDepth = projCoords.z;
  return currentDepth > closestDepth ? 1.0 : 0.0;
}

void main() {
  vec3 seaColor = vec3(104.0f/255.0f, 195.0f/255.0f, 192.0f/255.0f);
  vec3 fogColor = vec3(0.968f, 0.851f, 0.667f);

  // directional light
  vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
  vec3 lightDir = normalize(-lightPos);

  // ambient
  float ambientStrength = 0.15f;
  vec3 ambient = ambientStrength * lightColor;

  // diffuse
  float diff = max(dot(Normal, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;

  // float visibility
  float shadow = shadowCalculation(LightSpaceFragPos);
  vec3 fragColor = (ambient + (1 - shadow) * diffuse) * seaColor;

  // fog
  float dist = abs(ViewSpace.z);
  float near = 50.0f;
  float far = 200.0f;
  float fogFactor = (far - dist)/(far - near);
  fogFactor = clamp(fogFactor, 0.0, 1.0);

  vec3 finalColor = (1.0f - fogFactor) * fogColor + fogFactor * fragColor;
  out_Color = vec4(finalColor, 0.7f);
}
