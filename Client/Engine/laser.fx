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

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.viewPos = mul(float4(input.pos, 1.f), g_matWV);
    output.uv = input.uv;
    output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    // �⺻ ������ ���� (g_vec4_0���� �޾ƿ�)
    float4 color = g_vec4_0;
    
    // �����ڸ� ȿ�� ���
    float3 viewNormal = normalize(input.viewNormal);
    float3 viewDir = normalize(-input.viewPos.xyz);
    float rimFactor = 1.0f - saturate(dot(viewDir, viewNormal));
    rimFactor = pow(rimFactor, 2.0f); // ���� ����
    
    // �ð��� ���� �ĵ� ȿ��
    float wave = sin(input.uv.x * 10.0f + g_float_0 * 5.0f) * 0.5f + 0.5f;
    
    // �߱� ȿ�� (g_float_1���� ���� �޾ƿ�)
    float glowIntensity = g_float_1;
    
    // ���� ���� ���
    float4 finalColor = color;
    finalColor.rgb += color.rgb * rimFactor * 2.0f; // �����ڸ� �߱�
    finalColor.rgb += color.rgb * wave * 0.3f; // �ĵ� ȿ��
    finalColor.rgb *= glowIntensity; // ��ü���� �߱� ����
    
    // ���İ� ����
    finalColor.a = color.a;
    
    return finalColor;
}

#endif