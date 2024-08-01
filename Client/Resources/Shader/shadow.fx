#ifndef _SHADOW_FX_
#define _SHADOW_FX_

#include "params.fx"

struct VS_IN
{
    float3 pos : POSITION;
    float4 weight : WEIGHT;
    float4 indices : INDICES;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 clipPos : POSITION;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0.f;

    // 애니메이션 적용
    float3 pos = input.pos;
    if (g_int_1 == 1) // 애니메이션 여부 확인
    {
        float4 skinned = float4(0.f, 0.f, 0.f, 0.f);
        for (int i = 0; i < 4; ++i)
        {
            int index = input.indices[i];
            if (index < 0)
                break;

            skinned += mul(float4(pos, 1.f), g_mat_bone[index]) * input.weight[i];
        }
        pos = skinned.xyz;
    }

    output.pos = mul(float4(pos, 1.f), g_matWVP);
    output.clipPos = output.pos;
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    return float4(input.clipPos.z / input.clipPos.w, 0.f, 0.f, 0.f);
}

#endif