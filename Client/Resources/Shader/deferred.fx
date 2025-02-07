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

    // 스키닝 적용 여부(g_int_1)가 켜져 있으면 스키닝 진행
    if (g_int_1 == 1)
        Skinning(input.pos, input.normal, input.tangent, input.weight, input.indices);

    // 인스턴스 여부(g_int_0)에 따라 사용 행렬 결정
    matrix matWVP = (g_int_0 == 1) ? input.matWVP : g_matWVP;
    matrix matWV = (g_int_0 == 1) ? input.matWV : g_matWV;

    output.pos = mul(float4(input.pos, 1.f), matWVP);
    output.uv = input.uv;
    output.viewPos = mul(float4(input.pos, 1.f), matWV).xyz;
    
    // 법선 및 탄젠트를 뷰 공간으로 변환
    float3 viewNormal = normalize(mul(float4(input.normal, 0.f), matWV).xyz);
    float3 viewTangent = normalize(mul(float4(input.tangent, 0.f), matWV).xyz);
    
    // Gram-Schmidt 과정을 통해 tangent를 정규직교화: tangent에서 normal 성분을 제거
    viewTangent = normalize(viewTangent - dot(viewTangent, viewNormal) * viewNormal);
    // 오른손 좌표계를 위해 binormal을 normal과 tangent의 교차곱으로 계산 (일반적으로 binormal = cross(normal, tangent))
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

    // 기본 색상은 흰색; 텍스처 사용 여부에 따라 샘플링
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    if (g_tex_on_0 == 1)
        color = g_tex_0.Sample(g_sam_0, input.uv);

    float3 viewNormal = input.viewNormal;
    if (g_tex_on_1 == 1)
    {
        float3 tangentSpaceNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;
        tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.f;
        // 노멀맵 강도를 약간 낮춰 카툰 느낌 강화 (0.0: 사용안함, 1.0: 원래 효과)
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
// Outline Pass: 윤곽선 렌더링 (검은색 실루엣)
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
    
    // 뷰 공간에서의 위치와 normal 계산
    float3 viewPos = mul(float4(input.pos, 1.f), matWV).xyz;
    float3 viewNormal = normalize(mul(float4(input.normal, 0.f), matWV).xyz);
    
    // 윤곽선 두께 (카툰 스타일에서는 약간 확대한 모형 경계가 윤곽선)
    float outlineWidth = 0.03f;
    // 원래 위치에서 viewNormal 방향으로 오프셋
    float3 offsetPos = input.pos + viewNormal * outlineWidth;
    
    output.pos = mul(float4(offsetPos, 1.f), matWVP);
    output.uv = input.uv;
    return output;
}

PS_OUT PS_Outline(VS_OUT_OUTLINE input)
{
    PS_OUT output = (PS_OUT) 0;
    // 윤곽선은 단색(일반적으로 검은색)으로 처리
    output.color = float4(0, 0, 0, 1);
    return output;
}

#endif