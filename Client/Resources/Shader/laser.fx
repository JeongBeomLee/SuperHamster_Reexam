#ifndef _LASER_FX_
#define _LASER_FX_

#include "params.fx"

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 viewPos : POSITION;
    float2 uv : TEXCOORD;
    float3 viewNormal : NORMAL;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.viewPos = mul(float4(input.pos, 1.f), g_matWV);
    output.uv = input.uv;
    output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    // 기본 레이저 색상 (g_vec4_0에서 받아옴)
    float4 color = g_vec4_0;
    
    // 가장자리 효과 계산
    float3 viewNormal = normalize(input.viewNormal);
    float3 viewDir = normalize(-input.viewPos.xyz);
    float rimFactor = 1.0f - saturate(dot(viewDir, viewNormal));
    rimFactor = pow(rimFactor, 2.0f); // 강도 조절
    
    // 시간에 따른 파동 효과
    float wave = sin(input.uv.x * 10.0f + g_float_0 * 5.0f) * 0.5f + 0.5f;
    
    // 발광 효과 (g_float_1에서 강도 받아옴)
    float glowIntensity = g_float_1;
    
    // 최종 색상 계산
    float4 finalColor = color;
    finalColor.rgb += color.rgb * rimFactor * 2.0f; // 가장자리 발광
    finalColor.rgb += color.rgb * wave * 0.3f; // 파동 효과
    finalColor.rgb *= glowIntensity; // 전체적인 발광 강도
    
    // 알파값 유지
    finalColor.a = color.a;
    
    return finalColor;
}

#endif