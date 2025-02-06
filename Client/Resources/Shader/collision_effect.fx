#ifndef _COLLISION_EFFECT_FX_
#define _COLLISION_EFFECT_FX_

#include "params.fx"

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

// collision_effect.fx
struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 viewPos : POSITION;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    // 빌보드 행렬 계산
    float3 up = float3(0.f, 1.f, 0.f);
    float3 look = -g_vec4_1.xyz; // 카메라 방향 사용
    float3 right = normalize(cross(up, look));
    up = normalize(cross(look, right));
    
    float3 worldPos = input.pos.x * right + input.pos.y * up;
    
    output.pos = mul(float4(worldPos, 1.f), g_matWVP);
    output.uv = input.uv;
    output.viewPos = mul(float4(worldPos, 1.f), g_matView).xyz;
    
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = g_tex_0.Sample(g_sam_0, input.uv);
    
    // 진행도에 따른 페이드아웃
    float fadeOut = 1.0f - g_float_2;
    
    // 발광 효과
    color.rgb *= g_vec4_0.rgb * 2.0f;
    color.a *= fadeOut;
    
    return color;
}

#endif