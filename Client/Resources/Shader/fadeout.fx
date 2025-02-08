#ifndef _FADE_OUT_FX_
#define _FADE_OUT_FX_

#include "params.fx"

// ���� �Է� ����ü�� UV �߰� (�ؽ�ó ��ǥ �뵵)
struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD0; // 0~1 ������ ��ǥ�� �Ҵ�Ǿ� �ִٰ� ����
};

// ���� ��� ����ü���� UV�� �Բ� ����
struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    // ����-��-�������� ��ķ� ��ȯ
    output.pos = mul(float4(input.pos, 1.0), g_matWVP);
    
    // UV ��ǥ�� �״�� ����
    output.uv = input.uv;
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float fadeBias = 0.8f;
    float fadeScale = 10.0f;
    
    // ������ �׶��̼� ������ ����ȭ
    float2 gradDir = normalize(g_vec2_0);
    
    // UV ��ǥ�� �׶��̼� ������ �������� ���� ��ġ ���� (0~1 ����)
    float factor = dot(input.uv, gradDir);
    
    // bias�� scale�� �����Ͽ�, g_fadeBias ���Ͽ����� �������ϰ� �����ϰ�,
    // �� �̻���� ������ ������������ ��
    float adjusted = saturate((factor - fadeBias) * fadeScale);
    
    // ���İ� ���: adjusted�� 0�̸� 1(������), 1�̸� 0(���� ����)
    float alpha = 1.0 - adjusted;
    
    // ���� ������ �������� ���� ���İ� ����
    return float4(0, 0, 0, alpha);
}

#endif