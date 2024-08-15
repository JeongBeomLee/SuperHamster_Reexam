#ifndef _PARTICLE_FX_
#define _PARTICLE_FX_

#include "params.fx"
#include "utils.fx"

struct Particle
{
    float3 worldPos;
    float curTime;
    float3 worldDir;
    float lifeTime;
    int alive;
    float3 padding;
};

StructuredBuffer<Particle> g_data : register(t9);

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    uint id : SV_InstanceID;
};

struct VS_OUT
{
    float4 viewPos : POSITION;
    float2 uv : TEXCOORD;
    float id : ID;
};

// 상수 정의
static const float c_PI = 3.14159265359;

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0.f;

    float3 worldPos = mul(float4(input.pos, 1.f), g_matWorld).xyz;
    uint id = input.id;
    Particle particle = g_data[id];
    
    float t = particle.curTime;
    float maxRadius = 50.0; // 최대 반경

    if (particle.alive)
    {
        // 반딪불이의 랜덤한 움직임 구현
        float3 randomOffset = float3(
            sin(t * 2.0 + id * 0.1) * 0.5,
            cos(t * 1.5 + id * 0.2) * 0.3,
            sin(t * 1.8 + id * 0.3) * 0.4
        );

        worldPos = particle.worldPos + randomOffset * maxRadius;
    }

    output.viewPos = mul(float4(worldPos, 1.f), g_matView);
    output.uv = input.uv;
    output.id = id;

    return output;
}

struct GS_OUT
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
    uint id : SV_InstanceID;
    float brightness : BRIGHTNESS;
};

[maxvertexcount(6)]
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUT> outputStream)
{
    GS_OUT output[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };

    VS_OUT vtx = input[0];
    uint id = (uint) vtx.id;
    if (0 == g_data[id].alive)
        return;

    float ratio = g_data[id].curTime / g_data[id].lifeTime;
    float scale = ((g_float_1 - g_float_0) * ratio + g_float_0) / 2.f;

    // 반딪불이 깜박임 효과
    float blinkSpeed = 5.0;
    float brightness = (sin(g_data[id].curTime * blinkSpeed) + 1.0) * 0.5;

    // 파티클 크기 조절 (생성 시 작고, 중간에 커졌다가, 소멸 시 작아지도록)
    float sizeMultiplier = sin(ratio * c_PI);
    scale *= sizeMultiplier;

    // View Space
    output[0].position = vtx.viewPos + float4(-scale, scale, 0.f, 0.f);
    output[1].position = vtx.viewPos + float4(scale, scale, 0.f, 0.f);
    output[2].position = vtx.viewPos + float4(scale, -scale, 0.f, 0.f);
    output[3].position = vtx.viewPos + float4(-scale, -scale, 0.f, 0.f);

    // Projection Space
    output[0].position = mul(output[0].position, g_matProjection);
    output[1].position = mul(output[1].position, g_matProjection);
    output[2].position = mul(output[2].position, g_matProjection);
    output[3].position = mul(output[3].position, g_matProjection);

    output[0].uv = float2(0.f, 0.f);
    output[1].uv = float2(1.f, 0.f);
    output[2].uv = float2(1.f, 1.f);
    output[3].uv = float2(0.f, 1.f);

    output[0].id = id;
    output[1].id = id;
    output[2].id = id;
    output[3].id = id;

    output[0].brightness = brightness;
    output[1].brightness = brightness;
    output[2].brightness = brightness;
    output[3].brightness = brightness;

    outputStream.Append(output[0]);
    outputStream.Append(output[1]);
    outputStream.Append(output[2]);
    outputStream.RestartStrip();

    outputStream.Append(output[0]);
    outputStream.Append(output[2]);
    outputStream.Append(output[3]);
    outputStream.RestartStrip();
}

float4 PS_Main(GS_OUT input) : SV_Target
{
    float4 texColor = g_tex_0.Sample(g_sam_0, input.uv);
    
    // 반딪불이 색상 (연노란색)
    float3 fireflyColor = float3(1.0, 1.0, 0.5);
    
    // 밝기에 따른 색상 조정
    float4 finalColor = float4(fireflyColor , texColor.a);
    
    return finalColor;
}

struct ComputeShared
{
    int addCount;
    float3 padding;
};

RWStructuredBuffer<Particle> g_particle : register(u0);
RWStructuredBuffer<ComputeShared> g_shared : register(u1);

// 랜덤 함수
float rand(float2 co)
{
    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}

[numthreads(1024, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    if (threadIndex.x >= g_int_0)
        return;

    int maxCount = g_int_0;
    float deltaTime = g_vec2_1.x;
    float minLifeTime = g_vec4_0.x;
    float maxLifeTime = g_vec4_0.y;

    if (g_particle[threadIndex.x].alive == 0)
    {
        // 새 파티클 생성
        float3 randomPos = float3(
            (rand(float2(threadIndex.x, g_vec2_1.y)) * 2 - 1) * 50,
            (rand(float2(threadIndex.x + 1, g_vec2_1.y)) * 2 - 1) * 50,
            (rand(float2(threadIndex.x + 2, g_vec2_1.y)) * 2 - 1) * 50
        );

        g_particle[threadIndex.x].worldPos = randomPos;
        g_particle[threadIndex.x].lifeTime = lerp(minLifeTime, maxLifeTime, rand(float2(threadIndex.x + 3, g_vec2_1.y)));
        g_particle[threadIndex.x].curTime = 0.f;
        g_particle[threadIndex.x].alive = 1;
    }
    else
    {
        g_particle[threadIndex.x].curTime += deltaTime;
        if (g_particle[threadIndex.x].lifeTime < g_particle[threadIndex.x].curTime)
        {
            g_particle[threadIndex.x].alive = 0;
        }
    }
}

#endif