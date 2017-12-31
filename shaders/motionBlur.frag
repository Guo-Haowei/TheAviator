// motionBlur.frag
#version 330 core

uniform sampler2D colorTexture;
uniform sampler2D velocityTexture;
out vec4 out_Color;

void main() {
  vec2 texelSize = 1.0 / vec2(textureSize(colorTexture, 0));
  vec2 screenTexCoords = gl_FragCoord.xy * texelSize;

  vec2 velocity = texture(velocityTexture, screenTexCoords).rg;
  velocity.x = pow(velocity.x, 1.0 / 3.0);
  velocity.y = pow(velocity.y, 1.0 / 3.0);

  float speed = length(velocity / texelSize);
  int nSamples = clamp(int(speed), 1, 10);
  nSamples = 1;

  out_Color = texture(colorTexture, screenTexCoords);
  for (int i = 1; i < nSamples; ++i) {
    vec2 offset = velocity * (float(i) / float(nSamples - 1) - 0.5);
    out_Color += texture(colorTexture, screenTexCoords + offset);
  }
  out_Color /= float(nSamples);
}
