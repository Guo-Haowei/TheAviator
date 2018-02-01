// sea.frag
#version 330 core
in vec4 viewSpace;
in vec3 Normal;

out vec4 out_Color;

uniform vec3 lightPos;

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
  vec3 fragColor = (ambient + diffuse) * seaColor;

  // fog
  float dist = abs(viewSpace.z);
  float near = 50.0f;
  float far = 200.0f;
  float fogFactor = (far - dist)/(far - near);
  fogFactor = clamp(fogFactor, 0.0, 1.0);

  vec3 finalColor = (1.0f - fogFactor) * fogColor + fogFactor * fragColor;
  out_Color = vec4(finalColor, 0.7f);
}
