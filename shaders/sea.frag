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
  if (currentDepth > 1.0)
    return 0;
  float shadow = 0.0;
  float bias = max(0.05 * (1.0 - dot(Normal, normalize(-lightPos))), 0.001);
  vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
  int sampleSize = 1;
  for(int x = -sampleSize; x <= sampleSize; ++x) {
    for(int y = -sampleSize; y <= sampleSize; ++y) {
        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
  }
  return shadow / ((sampleSize * 2 + 1) * (sampleSize * 2 + 1));
}

void main() {
  vec3 seaColor = vec3(0.408, 0.765, 0.753);
  vec3 fogColor = vec3(0.968, 0.851, 0.667);

  // directional light
  vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
  vec3 lightDir = normalize(-lightPos);

  // ambient
  float ambientStrength = 0.15f;
  vec3 ambient = ambientStrength * lightColor;

  // diffuse
  float diff = max(dot(Normal, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;

  // shadow
  float visibility = 1.0;
  float shadow = visibility * shadowCalculation(LightSpaceFragPos);
  vec3 fragColor = (ambient + (1 - shadow) * diffuse) * seaColor;

  // fog
  float dist = abs(ViewSpace.z);
  float near = 50.0;
  float far = 170.0;
  float fogFactor = (far - dist)/(far - near);
  fogFactor = clamp(fogFactor, 0.0, 1.0);

  vec3 finalColor = (1.0 - fogFactor) * fogColor + fogFactor * fragColor;
  out_Color = vec4(finalColor, 0.7);
}
