// flare.vert
#version 330 core
in vec2 position;

out vec2 TextureCoords;

uniform vec4 transform;

void main(void){
  TextureCoords = position + vec2(0.5, 0.5);
  vec2 screenPosition = position * transform.zw / 2.0 + transform.xy;
  
  gl_Position = vec4(
    (screenPosition.x - 0.5) * 2.0,
    (screenPosition.y - 0.5) * -2.0,
    0.0, 1.0
    );
}