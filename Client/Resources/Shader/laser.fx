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

// uniform float4 g_vec4_0; // �⺻ ������ ����
// uniform float4 g_vec4_1; // ����(���̶���Ʈ) ������ ����
// uniform float2 g_vec2_0.x; // ���� �ӵ�
// uniform float  g_float_0; // �ð�
// uniform float  g_float_1; // �߱� ����
// uniform float  g_float_2; // �ְ�(�����) ����
// uniform float  g_float_3; // �ְ� ���ļ�

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    // �⺻ �ĵ� ȿ�� (UV.y�� ���� ������ ������)
    float waveOffset = sin(input.uv.y * 20.0f + g_float_0 * 5.0f) * 0.05f;

    // �߰� �ְ� ȿ��: �߰� ����(g_float_2, g_float_3)�� �̿��� ���� ȿ��
    float swirl = sin(input.uv.y * g_float_3 + g_float_0) * g_float_2;
    
    // tangent �������� �� ȿ���� ���ļ� ���� ��ġ�� �����մϴ�.
    float3 displacedPos = input.pos + input.tangent * (waveOffset + swirl);

    output.pos = mul(float4(displacedPos, 1.f), g_matWVP);
    output.viewPos = mul(float4(displacedPos, 1.f), g_matWV);
    output.uv = input.uv;
    output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    // �⺻ ����� ���� ������ �غ�
    float4 baseColor = g_vec4_0;
    float4 highlightColor = g_vec4_1;

    // ���� ����: �߰� ����(g_float_4)�� ����� �ð��� ���� �� ������ ȥ���մϴ�.
    float blendFactor = 0.5 + 0.5 * sin(g_float_0 * g_vec2_0.x + input.uv.x * 20.0f);
    float4 mixedColor = lerp(baseColor, highlightColor, blendFactor);

    // --- �⺻ ȿ�� ---
    // Rim lighting ��� (ī�޶���� ������ ���� �ֺ��� �߱�)
    float3 viewNormal = normalize(input.viewNormal);
    float3 viewDir = normalize(-input.viewPos.xyz);
    float rimFactor = pow(1.0f - saturate(dot(viewDir, viewNormal)), 2.0f);

    // ���� �� ���� �ĵ� ȿ��
    float horizontalWave = sin(input.uv.x * 10.0f + g_float_0 * 5.0f) * 0.5f + 0.5f;
    float verticalWave = sin(input.uv.y * 20.0f + g_float_0 * 7.0f) * 0.5f + 0.5f;

    // �߾ӿ��� �ٱ����� ������ ����� �׶��̼�
    float2 centeredUV = input.uv - 0.5f;
    float radial = smoothstep(0.5, 0.0, length(centeredUV));

    // �ð��� ���� �޽� ȿ��
    float pulse = 0.5f + 0.5f * sin(g_float_0 * 10.0f);

    // �⺻ ȿ������ ȥ���Ͽ� ���� ������ ���
    float4 finalColor = mixedColor;
    finalColor.rgb += mixedColor.rgb * rimFactor * 1.5f;
    finalColor.rgb += mixedColor.rgb * horizontalWave * 0.3f;
    finalColor.rgb += mixedColor.rgb * verticalWave * 0.2f;
    finalColor.rgb += mixedColor.rgb * radial * 0.5f;
    finalColor.rgb *= (0.8f + 0.4f * pulse);
    finalColor.rgb *= g_float_1; // �߱� ���� ����

    // --- �ܻ�(Ghost Trail) ȿ�� ---
    // ���� �ð��� �ణ�� �������� �־� ���� ghost pass�� ���ϱ�
    float ghostTrail1 = sin(input.uv.x * 10.0f + (g_float_0 - 0.1f) * 5.0f) * 0.5f + 0.5f;
    float ghostTrail2 = sin(input.uv.x * 10.0f + (g_float_0 - 0.2f) * 5.0f) * 0.5f + 0.5f;
    float ghostTrail3 = sin(input.uv.x * 10.0f + (g_float_0 - 0.3f) * 5.0f) * 0.5f + 0.5f;

    finalColor.rgb += mixedColor.rgb * ghostTrail1 * 0.3f;
    finalColor.rgb += mixedColor.rgb * ghostTrail2 * 0.25f;
    finalColor.rgb += mixedColor.rgb * ghostTrail3 * 0.2f;

    finalColor.a = mixedColor.a;

    return finalColor;
}

#endif