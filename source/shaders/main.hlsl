cbuffer cbPerObject : register(b0) { float4x4 gWorldViewProj; };

struct VertexIn {
  float3 position : POSITION;
  float3 normal : NORMAL;
  float4 color : COLOR;
};

struct VertexOut {
  float4 position : SV_POSITION;
  float3 normal : NORMAL;
  float4 color : COLOR;
};

VertexOut VS(VertexIn vin) {
  VertexOut vout;

  vout.position = mul(gWorldViewProj, float4(vin.position, 1.0f));
  vout.normal = vin.normal;
  vout.color = vin.color;

  return vout;
}

float4 PS(VertexOut pin) : SV_Target {
  float3 lightDir = float3(8.0, 10.0, -10.0);
  float power = max(dot(normalize(pin.normal), normalize(lightDir)), 0.0);
  float ambient = 0.1;
  power += ambient;
  float3 color = pin.color.rgb;
  return float4(color * power, pin.color.a);
}