// lava.fx
#ifndef _LAVA_FX_
#define _LAVA_FX_

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

// g_float_0: 경과 시간
// g_float_1: 파동 속도
// g_float_2: 파동 강도
// g_vec4_0: 베이스 컬러
// g_vec4_1: 하이라이트 컬러
// g_tex_0: 용암 텍스처
// g_tex_1: 노멀맵 텍스처

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;
    
    // 기본 위치
    float3 posModified = input.pos;
    
    // [버블링 효과]
    // 좌표에 따라 sin/cos 함수를 사용해 버블처럼 솟구치는 효과 적용 (Y축 방향)
    float wave1 = sin(input.pos.x * 10.0 + g_float_0 * g_float_1);
    float wave2 = cos(input.pos.z * 10.0 + g_float_0 * g_float_1);
    // 파동 강도(g_float_2)를 곱해 미세한 높이 변화를 적용
    float bubbleOffset = (wave1 + wave2) * 0.5 * g_float_2 * 0.05;
    posModified.y += bubbleOffset;
    
    // [스월(Swirl) 효과]
    // UV 좌표를 기준으로 화면 중앙에서 멀어질수록 살짝 회전하는 효과를 적용
    float2 center = float2(0.5, 0.5);
    float2 uvCentered = input.uv - center;
    float dist = length(uvCentered);
    
    // 회전량은 거리와 시간에 따라 변화
    float swirlAngle = sin(dist * 20.0 - g_float_0 * 2.0) * 0.05;
    // 회전 벡터 계산 (각도 기반 2D 회전)
    float cosA = cos(swirlAngle);
    float sinA = sin(swirlAngle);
    float2 rotatedUV = float2(
        uvCentered.x * cosA - uvCentered.y * sinA,
        uvCentered.x * sinA + uvCentered.y * cosA
    );
    
    // 회전에 따른 위치 오프셋을 적용 (X, Z 평면에 약간 반영)
    float2 swirlOffset = (rotatedUV - uvCentered) * 0.1;
    posModified.x += swirlOffset.x;
    posModified.z += swirlOffset.y;
    
    // 최종 변형된 위치를 월드-뷰-프로젝션 행렬로 변환
    output.pos = mul(float4(posModified, 1.0), g_matWVP);
    output.uv = input.uv;
    
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    // -- 기본 UV 왜곡: sin/cos를 이용한 파동 효과
    float2 uvBase = input.uv;
    float2 distortedUV;
    distortedUV.x = uvBase.x + sin(uvBase.y * 10.0 + g_float_0 * g_float_1) * g_float_2;
    distortedUV.y = uvBase.y + cos(uvBase.x * 10.0 + g_float_0 * g_float_1) * g_float_2;
    
    // -- 스월(Swirl) 효과: 화면 중심을 기준으로 회전 왜곡 적용
    float2 center = float2(0.5, 0.5);
    float2 uvCentered = input.uv - center;
    float radius = length(uvCentered);
    float angle = atan2(uvCentered.y, uvCentered.x);
    // 반지름에 따른 변동과 시간 기반 속도로 스월 효과 적용
    float swirlAmount = sin(radius * 15.0 + g_float_0 * g_float_1) * 0.03;
    angle += swirlAmount;
    float2 swirlUV = float2(cos(angle), sin(angle)) * radius + center;
    
    // -- 두 왜곡을 혼합: 부드러운 전환을 위해 선형 보간 사용
    float2 finalUV = lerp(distortedUV, swirlUV, 0.5);
    
    // -- 흐름(Flow) 효과: 시간이 지남에 따라 텍스처가 살짝 흘러가도록 함
    finalUV += float2(g_float_0 * 0.005, -g_float_0 * 0.003);
    
    // -- 기본 용암 텍스처 샘플링
    float4 baseColor = g_tex_0.Sample(g_sam_0, finalUV);
    
    // -- 노멀맵 샘플링 및 복원 ([-1,1] 범위로)
    float3 normalSample = g_tex_1.Sample(g_sam_0, finalUV).xyz;
    float3 normal = normalize(normalSample * 2.0 - 1.0);
    
    // -- 시간 및 위치에 따른 컬러 변조: 베이스 컬러와 하이라이트 컬러를 보간
    float colorLerp = sin(g_float_0 * 2.0 + radius * 10.0) * 0.5 + 0.5;
    float4 lavaColor = lerp(g_vec4_0, g_vec4_1, colorLerp);
    
    // -- 추가 하이라이트 효과: 노멀맵의 y 성분과 시간 기반 펄스 적용
    float highlight = pow(saturate(1.0 - abs(normal.y)), 2.0);
    highlight *= (0.5 + 0.5 * sin(g_float_0 * 3.0 + radius * 10.0));
    lavaColor += highlight * float4(1.0, 0.8, 0.5, 0.0);
    
    // -- 글로우 효과: 하이라이트가 강한 부분에서 발광 효과 강화
    float emissive = smoothstep(0.2, 0.8, highlight);
    lavaColor.rgb = lerp(lavaColor.rgb, lavaColor.rgb * 1.5, emissive);
    
    // -- 최종 색상 조합: 텍스처와 컬러 효과를 곱하고 알파도 시간에 따라 미세하게 변동
    float4 finalColor = lavaColor * baseColor;
    finalColor.a *= (0.8 + 0.2 * sin(g_float_0 * 4.0));
    
    return finalColor;
}
#endif