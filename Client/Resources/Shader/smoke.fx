// smoke.fx
#ifndef _SMOKE_FX_
#define _SMOKE_FX_

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

// g_float_0: ��� �ӵ�
// g_float_1: ���̵�ƿ� �ӵ�
// g_float_2: ���� ����
// g_vec4_0: ���� ����
// g_vec4_1: (x: ��� �ӵ�, y: ���̵�ƿ� �ӵ�, z: ���� ����)
// g_tex_0: ���� �ؽ�ó
// g_tex_1: ������ �ؽ�ó

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    // ������ ó��
    float3 up = float3(0.f, 1.f, 0.f);
    float3 look = normalize(float3(g_matView._13, g_matView._23, g_matView._33));
    float3 right = normalize(cross(up, look));
    
    float3 pos = input.pos.x * right + input.pos.y * up;
    output.pos = mul(float4(pos, 1.f), g_matWVP);
    output.uv = input.uv;
    
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float2 uv = input.uv;
    
    // �ð��� ���� ��� ȿ��
    uv.y -= g_vec4_1.x * g_float_0;
    
    // ������ ��� �ְ�
    float2 noiseUV = uv * 2.0f + float2(g_float_0 * 0.1f, g_float_0 * 0.2f);
    float noise = g_tex_1.Sample(g_sam_0, noiseUV).r;
    uv += (noise - 0.5f) * g_vec4_1.z;
    
    // ���� �ؽ�ó ���ø�
    float4 smoke = g_tex_0.Sample(g_sam_0, uv);
    
    // ���̵�ƿ� ȿ��
    float fadeOut = 1.0f - saturate(g_float_0 * g_vec4_1.y);
    
    // ���� ����
    float4 finalColor = g_vec4_0 * smoke * fadeOut;
    finalColor.a = smoke.a * fadeOut;
    
    return finalColor;
}

#endif