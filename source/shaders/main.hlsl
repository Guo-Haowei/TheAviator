cbuffer cbPerObject : register(b0) { float4x4 gWorld; };

cbuffer cbPass : register(b1) {
  float4x4 gView;
  float4x4 gInvView;
  float4x4 gProj;
  float4x4 gInvProj;
  float4x4 gViewProj;
  float4x4 gInvViewProj;
  float3 gEyePosW;
  float cbPerObjectPad1;
  float2 gRenderTargetSize;
  float2 gInvRenderTargetSize;
  float gNearZ;
  float gFarZ;
  float gTotalTime;
  float gDeltaTime;
};

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

  float4x4 PV = mul(gProj, gView);
  float4x4 PVW = mul(PV, gWorld);
  vout.position = mul(PVW, float4(vin.position, 1.0f));
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
