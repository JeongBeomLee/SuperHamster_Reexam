#ifndef _FADE_OUT_FX_
#define _FADE_OUT_FX_

#include "params.fx"

// 정점 입력 구조체에 UV 추가 (텍스처 좌표 용도)
struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD0; // 0~1 범위의 좌표가 할당되어 있다고 가정
};

// 정점 출력 구조체에도 UV를 함께 전달
struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    // 월드-뷰-프로젝션 행렬로 변환
    output.pos = mul(float4(input.pos, 1.0), g_matWVP);
    
    // UV 좌표를 그대로 전달
    output.uv = input.uv;
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float fadeBias = 0.8f;
    float fadeScale = 10.0f;
    
    // 지정된 그라데이션 방향을 정규화
    float2 gradDir = normalize(g_vec2_0);
    
    // UV 좌표와 그라데이션 방향의 내적으로 현재 위치 결정 (0~1 범위)
    float factor = dot(input.uv, gradDir);
    
    // bias와 scale을 적용하여, g_fadeBias 이하에서는 불투명하게 유지하고,
    // 그 이상부터 빠르게 투명해지도록 함
    float adjusted = saturate((factor - fadeBias) * fadeScale);
    
    // 알파값 계산: adjusted가 0이면 1(불투명), 1이면 0(완전 투명)
    float alpha = 1.0 - adjusted;
    
    // 최종 색상은 검은색에 계산된 알파값 적용
    return float4(0, 0, 0, alpha);
}

#endif