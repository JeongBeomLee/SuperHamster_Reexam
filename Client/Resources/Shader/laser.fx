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

// uniform float4 g_vec4_0; // 기본 레이저 색상
// uniform float4 g_vec4_1; // 보조(하이라이트) 레이저 색상
// uniform float2 g_vec2_0.x; // 블렌딩 속도
// uniform float  g_float_0; // 시간
// uniform float  g_float_1; // 발광 강도
// uniform float  g_float_2; // 왜곡(디스토션) 강도
// uniform float  g_float_3; // 왜곡 주파수

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    // 기본 파동 효과 (UV.y에 따른 사인파 오프셋)
    float waveOffset = sin(input.uv.y * 20.0f + g_float_0 * 5.0f) * 0.05f;

    // 추가 왜곡 효과: 추가 인자(g_float_2, g_float_3)를 이용한 스월 효과
    float swirl = sin(input.uv.y * g_float_3 + g_float_0) * g_float_2;
    
    // tangent 방향으로 두 효과를 합쳐서 정점 위치를 변위합니다.
    float3 displacedPos = input.pos + input.tangent * (waveOffset + swirl);

    output.pos = mul(float4(displacedPos, 1.f), g_matWVP);
    output.viewPos = mul(float4(displacedPos, 1.f), g_matWV);
    output.uv = input.uv;
    output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    // 기본 색상과 보조 색상을 준비
    float4 baseColor = g_vec4_0;
    float4 highlightColor = g_vec4_1;

    // 색상 블렌딩: 추가 인자(g_float_4)를 사용해 시간에 따라 두 색상을 혼합합니다.
    float blendFactor = 0.5 + 0.5 * sin(g_float_0 * g_vec2_0.x + input.uv.x * 20.0f);
    float4 mixedColor = lerp(baseColor, highlightColor, blendFactor);

    // --- 기본 효과 ---
    // Rim lighting 계산 (카메라와의 각도에 따른 주변부 발광)
    float3 viewNormal = normalize(input.viewNormal);
    float3 viewDir = normalize(-input.viewPos.xyz);
    float rimFactor = pow(1.0f - saturate(dot(viewDir, viewNormal)), 2.0f);

    // 수평 및 수직 파동 효과
    float horizontalWave = sin(input.uv.x * 10.0f + g_float_0 * 5.0f) * 0.5f + 0.5f;
    float verticalWave = sin(input.uv.y * 20.0f + g_float_0 * 7.0f) * 0.5f + 0.5f;

    // 중앙에서 바깥으로 퍼지는 방사형 그라데이션
    float2 centeredUV = input.uv - 0.5f;
    float radial = smoothstep(0.5, 0.0, length(centeredUV));

    // 시간에 따른 펄스 효과
    float pulse = 0.5f + 0.5f * sin(g_float_0 * 10.0f);

    // 기본 효과들을 혼합하여 최종 색상을 계산
    float4 finalColor = mixedColor;
    finalColor.rgb += mixedColor.rgb * rimFactor * 1.5f;
    finalColor.rgb += mixedColor.rgb * horizontalWave * 0.3f;
    finalColor.rgb += mixedColor.rgb * verticalWave * 0.2f;
    finalColor.rgb += mixedColor.rgb * radial * 0.5f;
    finalColor.rgb *= (0.8f + 0.4f * pulse);
    finalColor.rgb *= g_float_1; // 발광 강도 적용

    // --- 잔상(Ghost Trail) 효과 ---
    // 기존 시간에 약간의 오프셋을 주어 여러 ghost pass를 더하기
    float ghostTrail1 = sin(input.uv.x * 10.0f + (g_float_0 - 0.1f) * 5.0f) * 0.5f + 0.5f;
    float ghostTrail2 = sin(input.uv.x * 10.0f + (g_float_0 - 0.2f) * 5.0f) * 0.5f + 0.5f;
    float ghostTrail3 = sin(input.uv.x * 10.0f + (g_float_0 - 0.3f) * 5.0f) * 0.5f + 0.5f;

    finalColor.rgb += mixedColor.rgb * ghostTrail1 * 0.3f;
    finalColor.rgb += mixedColor.rgb * ghostTrail2 * 0.25f;
    finalColor.rgb += mixedColor.rgb * ghostTrail3 * 0.2f;

    finalColor.a = mixedColor.a;

    return finalColor;
}

#endif