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
    float amplitude = 50.0; // 일정한 반경
    float period = 1.0; // 파티클 주기 조절
    float maxHeight = 5.0; // 최대 높이

    if (t > 0)
    {
        t = particle.lifeTime * frac(t / particle.lifeTime);

        float angle = particle.curTime * 2.f * c_PI * period;
        float x = amplitude * cos(angle);
        float z = amplitude * sin(angle);
        float y = particle.worldPos.y - t * 10.0; // Y축으로 상승

        if (y < maxHeight) // 최대 높이 초과 방지
        {
            y = maxHeight;
        }

        worldPos.x = x;
        worldPos.y = y;
        worldPos.z = z;
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
    return texColor; // 텍스처 색상을 그대로 사용
}

struct ComputeShared
{
    int addCount;
    float3 padding;
};

RWStructuredBuffer<Particle> g_particle : register(u0);
RWStructuredBuffer<ComputeShared> g_shared : register(u1);

//// CS_Main
//// g_vec2_1 : DeltaTime / AccTime
//// g_int_0  : Particle Max Count
//// g_int_1  : AddCount
//// g_vec4_0 : MinLifeTime / MaxLifeTime / MinSpeed / MaxSpeed
//[numthreads(1024, 1, 1)]
//void CS_Main(int3 threadIndex : SV_DispatchThreadID)
//{
//    if (threadIndex.x >= g_int_0)
//        return;

//    int maxCount = g_int_0;
//    int addCount = g_int_1;
//    int frameNumber = g_int_2;
//    float deltaTime = g_vec2_1.x;
//    float accTime = g_vec2_1.y;
//    float minLifeTime = g_vec4_0.x;
//    float maxLifeTime = g_vec4_0.y;
//    float minSpeed = g_vec4_0.z;
//    float maxSpeed = g_vec4_0.w;

//    g_shared[0].addCount = addCount;
//    GroupMemoryBarrierWithGroupSync();

//    if (g_particle[threadIndex.x].alive == 0)
//    {
//        while (true)
//        {
//            int remaining = g_shared[0].addCount;
//            if (remaining <= 0)
//                break;

//            int expected = remaining;
//            int desired = remaining - 1;
//            int originalValue;
//            InterlockedCompareExchange(g_shared[0].addCount, expected, desired, originalValue);

//            if (originalValue == expected)
//            {
//                g_particle[threadIndex.x].alive = 1;
//                break;
//            }
//        }

//        if (g_particle[threadIndex.x].alive == 1)
//        {
//            // 초기 위치를 원의 둘레에서 설정
//            float angle = ((float) threadIndex.x / (float) maxCount) * 2.f * c_PI; // 일정한 각도
//            float radius = 25.0; // 초기 반경 크기
//            float3 pos = float3(radius * cos(angle), 0.0, radius * sin(angle));

//            g_particle[threadIndex.x].worldDir = float3(0.0, 1.0, 0.0); // Y축 방향으로 이동
//            g_particle[threadIndex.x].worldPos = pos; // 원의 둘레에서 시작
//            g_particle[threadIndex.x].lifeTime = minLifeTime; // 일정한 생명주기
//            g_particle[threadIndex.x].curTime = 0.f;
//        }
//    }
//    else
//    {
//        g_particle[threadIndex.x].curTime += deltaTime;
//        if (g_particle[threadIndex.x].lifeTime < g_particle[threadIndex.x].curTime)
//        {
//            g_particle[threadIndex.x].alive = 0;
//            return;
//        }

//        float speed = maxSpeed; // 일정한 속도로 이동

//        g_particle[threadIndex.x].worldPos += g_particle[threadIndex.x].worldDir * speed * deltaTime;
//    }
//}

#endif

