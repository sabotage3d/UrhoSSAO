
// Simple copy or blend of the ambient occlusion buffer. 
// Only for testing, ambient occlusion should only affect ambient light.

#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"
#include "ScreenPos.hlsl"

#line 10

void VS(float4 iPos : POSITION,
    out float2 oScreenPos : TEXCOORD0,
    out float4 oPos : POSITION)
{
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);
    oScreenPos = GetScreenPosPreDiv(oPos);
}

void PS(float2 iScreenPos : TEXCOORD0,
    out float4 oColor : COLOR0)
{
#if 0
    oColor = float4(tex2D(sEmissiveMap, iScreenPos).rgb, 1.0);
    return;
#endif

    float occlusion = tex2D(sEmissiveMap, iScreenPos).r;
#ifdef AO_ONLY
    float3 color = float3(occlusion,occlusion,occlusion);
#else
    float3 color = tex2D(sDiffMap, iScreenPos).rgb * occlusion;
#endif
    oColor = float4(color, 1.0);
}
