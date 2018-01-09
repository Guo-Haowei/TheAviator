// sea.frag
#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 ToCameraVector;

out vec4 out_Color;

void main() {
  vec3 seaColor = vec3(104.0f/255.0f, 195.0f/255.0f, 192.0f/255.0f);
  vec3 unitToCameraVector = normalize(ToCameraVector);

  // temp light properties
  vec3 lightPos = vec3(150, 350, 350);
  vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
  vec3 lightDir = normalize(lightPos);

  // ambient
  float ambientStrength = 0.3;
  vec3 ambient = ambientStrength * lightColor;

  // diffuse
  float diff = max(dot(Normal, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;

  out_Color = vec4((ambient + diffuse) * seaColor, 0.8f);
}
