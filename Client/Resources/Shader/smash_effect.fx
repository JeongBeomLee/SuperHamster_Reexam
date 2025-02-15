#ifndef _SMASH_EFFECT_FX_
#define _SMASH_EFFECT_FX_

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
    VS_OUT output;
    output.pos = mul(float4(input.pos, 1.0f), g_matWVP);
    output.uv = input.uv;
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    // ���� ����:
    // g_float_0: ��� �ð�
    // g_float_1: �� ���ӽð�
    // g_float_2: ���൵ (0~1)
    // g_vec4_0: �⺻ ���� (�����)
    //
    // �̹� ���������� �߰����� ���� ���� �Ʒ� ȿ���� �߰���:
    // 1. UV ��ǥ�� ���� �̼��� �ĵ�(����) �ְ�.
    // 2. ���� ���൵(g_float_2)�� ���� Ȯ���ϴ� ���� �� ����ũ.
    // 3. �ؽ�ó ����� �⺻ ������ ���� ���� ������ ����.
    
    // ȭ�� �߾� ���� ��ǥ ���
    float2 center = float2(0.5, 0.5);
    float2 centeredUV = input.uv - center;
    float dist = length(centeredUV);

    // [���� ȿ��]
    // ���൵�� ��������(�ʱ��ϼ���) ���� ���� ȿ��, �߽ɿ��� �־������� ����
    float maxSwirl = 6.28318; // 2*PI, �ִ� ���� ����
    float swirlAngle = (1.0 - g_float_2) * maxSwirl * exp(-dist * 5.0);
    float baseAngle = atan2(centeredUV.y, centeredUV.x);
    float newAngle = baseAngle + swirlAngle;
    float2 swirlUV = float2(cos(newAngle), sin(newAngle)) * dist;

    // [����(�ĵ�) ȿ��]
    // �ð�(g_float_0) ������� UV ��ǥ�� �̼��� ������ �߰�
    float rippleAmplitude = 0.005;
    float rippleFreq = 20.0;
    float ripple = sin((dist - g_float_2) * rippleFreq + g_float_0 * 3.0) * rippleAmplitude;
    float2 dir = (dist > 0.0001) ? (swirlUV / dist) : float2(0.0, 0.0);
    float2 finalUV = swirlUV + dir * ripple + center;

    // [�ؽ�ó ���ø�]
    float4 texColor = g_tex_0.Sample(g_sam_0, finalUV);
    
    // [�߰� �۷ο� ȿ��]
    // �ణ�� �������� �� �߰� ���ø����� ������ �۷ο�(�� ȿ��)�� �ο�
    float glowOffset = 0.003;
    float4 glowColor = g_tex_0.Sample(g_sam_0, finalUV + float2(glowOffset, glowOffset));
    float4 combinedColor = lerp(texColor, glowColor, 0.3);
    
    // [�� ����ũ ȿ��]
    // ���൵(g_float_2)�� �������� �ε巯�� ������ ���� ���� ����
    float ringWidth = 0.1;
    float ringMask = smoothstep(g_float_2 - ringWidth, g_float_2, dist)
                    * (1.0 - smoothstep(g_float_2, g_float_2 + ringWidth, dist));
    
    // ���� ����: �ؽ�ó ����, �� ����ũ, �⺻ ������ ���ϰ� ���൵�� ���� ���̵� �ƿ� ����
    float4 color = combinedColor * ringMask * g_vec4_0;
    color.a *= (1.0 - g_float_2);
    
    return color;
}

#endif
