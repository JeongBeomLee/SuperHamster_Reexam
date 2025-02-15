#ifndef _SMASH_EFFECT_FX_
#define _SMASH_EFFECT_FX_

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
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;
    output.pos = mul(float4(input.pos, 1.0f), g_matWVP);
    output.uv = input.uv;
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    // 기존 인자:
    // g_float_0: 경과 시간
    // g_float_1: 총 지속시간
    // g_float_2: 진행도 (0~1)
    // g_vec4_0: 기본 색상 (보라색)
    //
    // 이번 개선에서는 추가적인 인자 없이 아래 효과를 추가함:
    // 1. UV 좌표에 따른 미세한 파동(리플) 왜곡.
    // 2. 현재 진행도(g_float_2)에 맞춰 확장하는 원형 링 마스크.
    // 3. 텍스처 색상과 기본 색상을 곱해 최종 색상을 생성.
    
    // 화면 중앙 기준 좌표 계산
    float2 center = float2(0.5, 0.5);
    float2 centeredUV = input.uv - center;
    float dist = length(centeredUV);

    // [스월 효과]
    // 진행도가 낮을수록(초기일수록) 강한 스월 효과, 중심에서 멀어질수록 감쇠
    float maxSwirl = 6.28318; // 2*PI, 최대 스월 각도
    float swirlAngle = (1.0 - g_float_2) * maxSwirl * exp(-dist * 5.0);
    float baseAngle = atan2(centeredUV.y, centeredUV.x);
    float newAngle = baseAngle + swirlAngle;
    float2 swirlUV = float2(cos(newAngle), sin(newAngle)) * dist;

    // [리플(파동) 효과]
    // 시간(g_float_0) 기반으로 UV 좌표에 미세한 진동을 추가
    float rippleAmplitude = 0.005;
    float rippleFreq = 20.0;
    float ripple = sin((dist - g_float_2) * rippleFreq + g_float_0 * 3.0) * rippleAmplitude;
    float2 dir = (dist > 0.0001) ? (swirlUV / dist) : float2(0.0, 0.0);
    float2 finalUV = swirlUV + dir * ripple + center;

    // [텍스처 샘플링]
    float4 texColor = g_tex_0.Sample(g_sam_0, finalUV);
    
    // [추가 글로우 효과]
    // 약간의 오프셋을 준 추가 샘플링으로 은은한 글로우(빛 효과)를 부여
    float glowOffset = 0.003;
    float4 glowColor = g_tex_0.Sample(g_sam_0, finalUV + float2(glowOffset, glowOffset));
    float4 combinedColor = lerp(texColor, glowColor, 0.3);
    
    // [링 마스크 효과]
    // 진행도(g_float_2)를 기준으로 부드러운 에지의 원형 링을 생성
    float ringWidth = 0.1;
    float ringMask = smoothstep(g_float_2 - ringWidth, g_float_2, dist)
                    * (1.0 - smoothstep(g_float_2, g_float_2 + ringWidth, dist));
    
    // 최종 색상: 텍스처 샘플, 링 마스크, 기본 색상을 곱하고 진행도에 따라 페이드 아웃 적용
    float4 color = combinedColor * ringMask * g_vec4_0;
    color.a *= (1.0 - g_float_2);
    
    return color;
}

#endif
