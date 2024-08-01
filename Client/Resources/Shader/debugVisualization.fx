#ifndef _DEBUG_VISUALIZATION_FX_
#define _DEBUG_VISUALIZATION_FX_

#include "params.fx"

struct VS_IN
{
    float3 pos : POSITION;
};

struct VS_OUT
{
    float4 pos : SV_Position;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    return g_vec4_0; // 색상은 material->SetVec4로 설정
}

#endif