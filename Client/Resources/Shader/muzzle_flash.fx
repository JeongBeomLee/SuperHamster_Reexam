// muzzle_flash.fx
#ifndef _MUZZLE_FLASH_FX_
#define _MUZZLE_FLASH_FX_

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
    
    // �ܼ��� World-View-Projection ��ȯ�� ����
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
    
    // �߽ɿ����� �Ÿ� ���
    float2 centerUV = input.uv - 0.5f;
    float dist = length(centerUV);
    
    // �߽ɺ� �߱� ȿ��
    float centerGlow = 1.0f - smoothstep(0.0f, 0.2f, dist);
    centerGlow = pow(centerGlow, 4.0f); // �� ���� �߽ɺ� �߱�
    
    // �ð��� ���� ���̵�ƿ�
    float fadeOut = 1.0f - g_float_2; // ���൵�� ���� ���� �����ϰ�
    
    // ���� ����
    color.rgb = lerp(g_vec4_0.rgb, float3(1.0f, 1.0f, 1.0f), centerGlow); // �߽ɺδ� �������
    color.rgb *= 2.0f; // ��ü���� ��� ����
    
    // ���İ� ����
    color.a *= fadeOut;
    
    return color;
}

#endif