// lava.fx
#ifndef _LAVA_FX_
#define _LAVA_FX_

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

// g_float_0: ��� �ð�
// g_float_1: �ĵ� �ӵ�
// g_float_2: �ĵ� ����
// g_vec4_0: ���̽� �÷�
// g_vec4_1: ���̶���Ʈ �÷�
// g_tex_0: ��� �ؽ�ó
// g_tex_1: ��ָ� �ؽ�ó

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;
    
    // �⺻ ��ġ
    float3 posModified = input.pos;
    
    // [���� ȿ��]
    // ��ǥ�� ���� sin/cos �Լ��� ����� ����ó�� �ڱ�ġ�� ȿ�� ���� (Y�� ����)
    float wave1 = sin(input.pos.x * 10.0 + g_float_0 * g_float_1);
    float wave2 = cos(input.pos.z * 10.0 + g_float_0 * g_float_1);
    // �ĵ� ����(g_float_2)�� ���� �̼��� ���� ��ȭ�� ����
    float bubbleOffset = (wave1 + wave2) * 0.5 * g_float_2 * 0.05;
    posModified.y += bubbleOffset;
    
    // [����(Swirl) ȿ��]
    // UV ��ǥ�� �������� ȭ�� �߾ӿ��� �־������� ��¦ ȸ���ϴ� ȿ���� ����
    float2 center = float2(0.5, 0.5);
    float2 uvCentered = input.uv - center;
    float dist = length(uvCentered);
    
    // ȸ������ �Ÿ��� �ð��� ���� ��ȭ
    float swirlAngle = sin(dist * 20.0 - g_float_0 * 2.0) * 0.05;
    // ȸ�� ���� ��� (���� ��� 2D ȸ��)
    float cosA = cos(swirlAngle);
    float sinA = sin(swirlAngle);
    float2 rotatedUV = float2(
        uvCentered.x * cosA - uvCentered.y * sinA,
        uvCentered.x * sinA + uvCentered.y * cosA
    );
    
    // ȸ���� ���� ��ġ �������� ���� (X, Z ��鿡 �ణ �ݿ�)
    float2 swirlOffset = (rotatedUV - uvCentered) * 0.1;
    posModified.x += swirlOffset.x;
    posModified.z += swirlOffset.y;
    
    // ���� ������ ��ġ�� ����-��-�������� ��ķ� ��ȯ
    output.pos = mul(float4(posModified, 1.0), g_matWVP);
    output.uv = input.uv;
    
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    // -- �⺻ UV �ְ�: sin/cos�� �̿��� �ĵ� ȿ��
    float2 uvBase = input.uv;
    float2 distortedUV;
    distortedUV.x = uvBase.x + sin(uvBase.y * 10.0 + g_float_0 * g_float_1) * g_float_2;
    distortedUV.y = uvBase.y + cos(uvBase.x * 10.0 + g_float_0 * g_float_1) * g_float_2;
    
    // -- ����(Swirl) ȿ��: ȭ�� �߽��� �������� ȸ�� �ְ� ����
    float2 center = float2(0.5, 0.5);
    float2 uvCentered = input.uv - center;
    float radius = length(uvCentered);
    float angle = atan2(uvCentered.y, uvCentered.x);
    // �������� ���� ������ �ð� ��� �ӵ��� ���� ȿ�� ����
    float swirlAmount = sin(radius * 15.0 + g_float_0 * g_float_1) * 0.03;
    angle += swirlAmount;
    float2 swirlUV = float2(cos(angle), sin(angle)) * radius + center;
    
    // -- �� �ְ��� ȥ��: �ε巯�� ��ȯ�� ���� ���� ���� ���
    float2 finalUV = lerp(distortedUV, swirlUV, 0.5);
    
    // -- �帧(Flow) ȿ��: �ð��� ������ ���� �ؽ�ó�� ��¦ �귯������ ��
    finalUV += float2(g_float_0 * 0.005, -g_float_0 * 0.003);
    
    // -- �⺻ ��� �ؽ�ó ���ø�
    float4 baseColor = g_tex_0.Sample(g_sam_0, finalUV);
    
    // -- ��ָ� ���ø� �� ���� ([-1,1] ������)
    float3 normalSample = g_tex_1.Sample(g_sam_0, finalUV).xyz;
    float3 normal = normalize(normalSample * 2.0 - 1.0);
    
    // -- �ð� �� ��ġ�� ���� �÷� ����: ���̽� �÷��� ���̶���Ʈ �÷��� ����
    float colorLerp = sin(g_float_0 * 2.0 + radius * 10.0) * 0.5 + 0.5;
    float4 lavaColor = lerp(g_vec4_0, g_vec4_1, colorLerp);
    
    // -- �߰� ���̶���Ʈ ȿ��: ��ָ��� y ���а� �ð� ��� �޽� ����
    float highlight = pow(saturate(1.0 - abs(normal.y)), 2.0);
    highlight *= (0.5 + 0.5 * sin(g_float_0 * 3.0 + radius * 10.0));
    lavaColor += highlight * float4(1.0, 0.8, 0.5, 0.0);
    
    // -- �۷ο� ȿ��: ���̶���Ʈ�� ���� �κп��� �߱� ȿ�� ��ȭ
    float emissive = smoothstep(0.2, 0.8, highlight);
    lavaColor.rgb = lerp(lavaColor.rgb, lavaColor.rgb * 1.5, emissive);
    
    // -- ���� ���� ����: �ؽ�ó�� �÷� ȿ���� ���ϰ� ���ĵ� �ð��� ���� �̼��ϰ� ����
    float4 finalColor = lavaColor * baseColor;
    finalColor.a *= (0.8 + 0.2 * sin(g_float_0 * 4.0));
    
    return finalColor;
}
#endif