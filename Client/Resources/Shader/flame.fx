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

// g_float_0: ������ �ӵ�
// g_float_1: ������ ����
// g_float_2: ���� ������ �ӵ�
// g_float_3: ������ ������
// g_vec4_0: �⺻ ����
// g_vec4_1: ���κ� ����
// g_vec4_2: (x: ������ �ӵ�, y: ������ ����, z: ���� ������ �ӵ�, w: ������ ������)
// g_tex_0: �Ҳ� ���̽� �ؽ�ó
// g_tex_1: ������ �ؽ�ó
// g_tex_2: �׶��̼� ����ũ

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
    output.viewPos = mul(float4(pos, 1.f), g_matWV).xyz;
    
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    // �ð� ��� �ִϸ��̼�
    float time = g_vec4_2.x * g_float_0;
    
    // ������ ���ø� �� �ְ�
    float2 noiseUV = input.uv + float2(0, time * g_vec4_2.z);
    float noise = g_tex_1.Sample(g_sam_0, noiseUV * g_vec4_2.w).r;
    
    // UV �ְ�
    float2 distortedUV = input.uv;
    distortedUV.x += noise * 0.1f;
    
    // ���̽� �ؽ�ó�� �׶��̼�
    float4 baseColor = g_tex_0.Sample(g_sam_0, distortedUV);
    float gradient = g_tex_2.Sample(g_sam_0, float2(input.uv.x, input.uv.y)).r;
    
    // ���� ����
    float4 color = lerp(g_vec4_0, g_vec4_1, gradient);
    
    // ������ ȿ��
    float flicker = sin(time * g_vec4_2.x) * g_vec4_2.y + 1.0f;
    
    // ���� ����
    float4 finalColor = color * baseColor * flicker;
    finalColor.a = baseColor.a;
    
    return finalColor;
}

#endif