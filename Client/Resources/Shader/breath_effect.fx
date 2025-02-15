#ifndef _BREATH_EFFECT_FX_
#define _BREATH_EFFECT_FX_

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
    float3 worldPos : POSITION;
};

// ���� ���� �����ϴ� �Լ� (�Է� ��ǥ�� ���� -1 ~ 1 ������ ��ȯ)
float Rand(float2 co)
{
    return (frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453) * 2.0f - 1.0f);
}

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    // ���� ���� ���ļ� �ְ� ȿ��
    float waveX = sin(input.uv.x * 15.0f + g_float_0 * 7.0f) * 0.08f;
    float waveZ = cos(input.uv.y * 20.0f + g_float_0 * 5.0f) * 0.05f;
    float waveOffset = waveX + waveZ;
    
    float3 newPos = input.pos;
    newPos.xz += waveOffset;
    
    // �߰� ���� ȿ�� (g_vec4_3.x�� ����)
    newPos.x += sin(input.uv.y * 25.0f + g_float_0 * 3.0f) * g_vec4_3.x * 0.05f;
    newPos.z += cos(input.uv.x * 25.0f + g_float_0 * 3.0f) * g_vec4_3.x * 0.05f;
    
    // Rand �Լ��� Ȱ���� �߰� ������ �ְ� (���ؽ� ���� jitter)
    float randDistX = Rand(input.uv * 5.0f + float2(g_float_0, 0.0f));
    float randDistZ = Rand(input.uv * 5.0f + float2(0.0f, g_float_0));
    newPos.x += randDistX * 0.03f;
    newPos.z += randDistZ * 0.03f;
    
    output.pos = mul(float4(newPos, 1.0f), g_matWVP);
    output.uv = input.uv;
    output.worldPos = mul(float4(newPos, 1.0f), g_matWorld).xyz;
    
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    // UV ȸ�� ȿ�� (�ð��� swirl speed g_vec4_3.y �ݿ�)
    float angle = g_float_0 * g_vec4_3.y;
    float2 center = float2(0.5f, 0.5f);
    float2 uvCentered = input.uv - center;
    float cosA = cos(angle);
    float sinA = sin(angle);
    float2 rotatedUV = float2(uvCentered.x * cosA - uvCentered.y * sinA,
                              uvCentered.x * sinA + uvCentered.y * cosA) + center;
    
    // ������ �ؽ�ó ���ø����� �⺻ ������ �߰�
    float noiseSample = g_tex_0.Sample(g_sam_0, rotatedUV).r;
    
    // ���� ������ ��� UV �ְ �߰��� Rand ��� �̼� �ְ� ����
    rotatedUV += (noiseSample - 0.5f) * 0.05f;
    float randOffsetX = Rand(rotatedUV * 10.0f + float2(g_float_0, 0.0f)) * 0.02f;
    float randOffsetY = Rand(rotatedUV * 10.0f + float2(0.0f, g_float_0)) * 0.02f;
    rotatedUV += float2(randOffsetX, randOffsetY);
    
    // �߽ɿ����� �Ÿ��� �������� �÷� �׶��̼� ����
    float dist = length(rotatedUV - center);
    float t1 = smoothstep(0.0f, 0.4f, dist);
    float t2 = smoothstep(0.3f, 1.0f, dist);
    
    float4 innerColor = g_vec4_0; // �߽� ����
    float4 midColor = g_vec4_1; // �߰� ����
    float4 outerColor = g_vec4_2; // �ܰ� ���� (����)
    
    float4 colorGradient = lerp(innerColor, midColor, t1);
    colorGradient = lerp(colorGradient, outerColor, t2);
    
    // ���� ������ ȿ���� Rand ��� ���� ��ⷹ�̼� �߰�
    float flicker = (sin(input.uv.x * 25.0f + g_float_0 * 10.0f) +
                     cos(input.uv.y * 25.0f + g_float_0 * 8.0f)) * 0.5f;
    float randomFactor = Rand(input.uv * 20.0f + float2(g_float_0, g_float_0)) * 0.2f + 1.0f;
    flicker = saturate(flicker * g_vec4_3.z * randomFactor);
    colorGradient.rgb += flicker * 0.1f;
    
    // Rand �Լ��� �̿��� �߰� ����Ŭ(��¦��) ȿ��
    float sparkle = saturate(Rand(rotatedUV * 15.0f + float2(g_float_0, g_float_0)) * 2.0f);
    colorGradient.rgb += sparkle * 0.05f;
    
    // �߰� ������ ������ (�ؽ�ó ���ø� ��� Ȱ��)
    float detail = noiseSample * 0.1f;
    colorGradient.rgb += detail;
    
    // ���� ���̵�: ���൵(g_float_2)�� ������ ������, �Ÿ�(dist)�� �ݿ�
    float alpha = exp(-dist * 5.0f) * (1.0f - g_float_2);
    alpha *= (15.0f + noiseSample * 0.2f);
    colorGradient.a *= alpha;
    
    return colorGradient;
}
#endif