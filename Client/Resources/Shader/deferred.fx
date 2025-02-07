#ifndef _DEFAULT_FX_
#define _DEFAULT_FX_

#include "params.fx"
#include "utils.fx"

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 weight : WEIGHT;
    float4 indices : INDICES;

    row_major matrix matWorld : W;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;
    
    uint instanceID : SV_InstanceID;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    float3 viewBinormal : BINORMAL;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    // ��Ű�� ���� ����(g_int_1)�� ���� ������ ��Ű�� ����
    if (g_int_1 == 1)
        Skinning(input.pos, input.normal, input.tangent, input.weight, input.indices);

    // �ν��Ͻ� ����(g_int_0)�� ���� ��� ��� ����
    matrix matWVP = (g_int_0 == 1) ? input.matWVP : g_matWVP;
    matrix matWV = (g_int_0 == 1) ? input.matWV : g_matWV;

    output.pos = mul(float4(input.pos, 1.f), matWVP);
    output.uv = input.uv;
    output.viewPos = mul(float4(input.pos, 1.f), matWV).xyz;
    
    // ���� �� ź��Ʈ�� �� �������� ��ȯ
    float3 viewNormal = normalize(mul(float4(input.normal, 0.f), matWV).xyz);
    float3 viewTangent = normalize(mul(float4(input.tangent, 0.f), matWV).xyz);
    
    // Gram-Schmidt ������ ���� tangent�� ��������ȭ: tangent���� normal ������ ����
    viewTangent = normalize(viewTangent - dot(viewTangent, viewNormal) * viewNormal);
    // ������ ��ǥ�踦 ���� binormal�� normal�� tangent�� ���������� ��� (�Ϲ������� binormal = cross(normal, tangent))
    float3 viewBinormal = normalize(cross(viewNormal, viewTangent));
    
    output.viewNormal = viewNormal;
    output.viewTangent = viewTangent;
    output.viewBinormal = viewBinormal;
    
    return output;
}

struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
};

PS_OUT PS_Main(VS_OUT input)
{
    PS_OUT output = (PS_OUT) 0;

    // �⺻ ������ ���; �ؽ�ó ��� ���ο� ���� ���ø�
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    if (g_tex_on_0 == 1)
        color = g_tex_0.Sample(g_sam_0, input.uv);

    float3 viewNormal = input.viewNormal;
    if (g_tex_on_1 == 1)
    {
        float3 tangentSpaceNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;
        tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.f;
        // ��ָ� ������ �ణ ���� ī�� ���� ��ȭ (0.0: ������, 1.0: ���� ȿ��)
        float normalMapIntensity = 0.7f;
        tangentSpaceNormal = lerp(float3(0, 0, 1), tangentSpaceNormal, normalMapIntensity);
        float3x3 matTBN = { input.viewTangent, input.viewBinormal, input.viewNormal };
        viewNormal = normalize(mul(tangentSpaceNormal, matTBN));;
    }
    
    output.position = float4(input.viewPos, 0.f);
    output.normal = float4(viewNormal, 0.f);
    output.color = color;

    return output;
}

//---------------------------------------------------------
// Outline Pass: ������ ������ (������ �Ƿ翧)
//---------------------------------------------------------
struct VS_OUT_OUTLINE
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

VS_OUT_OUTLINE VS_Outline(VS_IN input)
{
    VS_OUT_OUTLINE output = (VS_OUT_OUTLINE) 0;
    
    if (g_int_1 == 1)
        Skinning(input.pos, input.normal, input.tangent, input.weight, input.indices);
    
    matrix matWVP = (g_int_0 == 1) ? input.matWVP : g_matWVP;
    matrix matWV = (g_int_0 == 1) ? input.matWV : g_matWV;
    
    // �� ���������� ��ġ�� normal ���
    float3 viewPos = mul(float4(input.pos, 1.f), matWV).xyz;
    float3 viewNormal = normalize(mul(float4(input.normal, 0.f), matWV).xyz);
    
    // ������ �β� (ī�� ��Ÿ�Ͽ����� �ణ Ȯ���� ���� ��谡 ������)
    float outlineWidth = 0.03f;
    // ���� ��ġ���� viewNormal �������� ������
    float3 offsetPos = input.pos + viewNormal * outlineWidth;
    
    output.pos = mul(float4(offsetPos, 1.f), matWVP);
    output.uv = input.uv;
    return output;
}

PS_OUT PS_Outline(VS_OUT_OUTLINE input)
{
    PS_OUT output = (PS_OUT) 0;
    // �������� �ܻ�(�Ϲ������� ������)���� ó��
    output.color = float4(0, 0, 0, 1);
    return output;
}

#endif