// shockwave.fx
#ifndef _SHOCKWAVE_FX_
#define _SHOCKWAVE_FX_

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
    VS_OUT output = (VS_OUT) 0;
    
    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;
    
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    // g_float_0: ��� �ð�
    // g_float_1: �� ���ӽð�
    // g_float_2: ���൵ (0~1)
    // g_vec4_0: ����

    // �ؽ�ó ���ø�
    float4 color = g_tex_0.Sample(g_sam_0, input.uv);
    
    // �ð��� ���� ���̵�ƿ�
    float fadeOut = 1.0f - g_float_2; // ���൵�� ���� ���� �����ϰ�
    
    // ���İ� ����
    color.a *= fadeOut;
    
    return color;
}

#endif