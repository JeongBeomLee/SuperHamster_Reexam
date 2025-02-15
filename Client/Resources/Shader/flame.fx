// flame.fx
#ifndef _FLAME_FX_
#define _FLAME_FX_

#include "params.fx"

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 viewPos : POSITION;
};

// g_float_0: 깜빡임 속도
// g_float_1: 깜빡임 강도
// g_float_2: 수직 움직임 속도
// g_float_3: 노이즈 스케일
// g_vec4_0: 기본 색상
// g_vec4_1: 끝부분 색상
// g_vec4_2: (x: 깜빡임 속도, y: 깜빡임 강도, z: 수직 움직임 속도, w: 노이즈 스케일)
// g_tex_0: 불꽃 베이스 텍스처
// g_tex_1: 노이즈 텍스처
// g_tex_2: 그라데이션 마스크

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    // 빌보드 처리
    float3 up = float3(0.f, 1.f, 0.f);
    float3 look = normalize(float3(g_matView._13, g_matView._23, g_matView._33));
    float3 right = normalize(cross(up, look));
    
    float3 pos = input.pos.x * right + input.pos.y * up;
    
    output.pos = mul(float4(pos, 1.f), g_matWVP);
    output.uv = input.uv;
    output.viewPos = mul(float4(pos, 1.f), g_matWV).xyz;
    
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    // 시간 기반 애니메이션
    float time = g_vec4_2.x * g_float_0;
    
    // 노이즈 샘플링 및 왜곡
    float2 noiseUV = input.uv + float2(0, time * g_vec4_2.z);
    float noise = g_tex_1.Sample(g_sam_0, noiseUV * g_vec4_2.w).r;
    
    // UV 왜곡
    float2 distortedUV = input.uv;
    distortedUV.x += noise * 0.1f;
    
    // 베이스 텍스처와 그라데이션
    float4 baseColor = g_tex_0.Sample(g_sam_0, distortedUV);
    float gradient = g_tex_2.Sample(g_sam_0, float2(input.uv.x, input.uv.y)).r;
    
    // 색상 블렌딩
    float4 color = lerp(g_vec4_0, g_vec4_1, gradient);
    
    // 깜빡임 효과
    float flicker = sin(time * g_vec4_2.x) * g_vec4_2.y + 1.0f;
    
    // 최종 색상
    float4 finalColor = color * baseColor * flicker;
    finalColor.a = baseColor.a;
    
    return finalColor;
}

#endif