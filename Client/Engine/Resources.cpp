#include "pch.h"
#include "Resources.h"
#include "Engine.h"
#include "MeshData.h"
#include "SoundSystem.h"

void Resources::Init()
{
	CreateDefaultShader();
	CreateDefaultMaterial();
	GET_SINGLE(SoundSystem)->Init();
	CreateDefaultSound();
}

shared_ptr<Mesh> Resources::LoadPointMesh()
{
	shared_ptr<Mesh> findMesh = Get<Mesh>(L"Point");
	if (findMesh)
		return findMesh;

	vector<Vertex> vec(1);
	vec[0] = Vertex(Vec3(0, 0, 0), Vec2(0.5f, 0.5f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));

	vector<uint32> idx(1);
	idx[0] = 0;

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Create(vec, idx);
	Add(L"Point", mesh);

	return mesh;
}

shared_ptr<Mesh> Resources::LoadLineMesh()
{
	shared_ptr<Mesh> findMesh = Get<Mesh>(L"Line");
	if (findMesh)
		return findMesh;

	vector<Vertex> vec(2);
	vec[0] = Vertex(Vec3(0, 0, 0), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[1] = Vertex(Vec3(0, 0, 1), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));

	vector<uint32> idx(2);
	idx[0] = 0;
	idx[1] = 1;

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Create(vec, idx);
	Add(L"Line", mesh);

	return mesh;
}

shared_ptr<Mesh> Resources::LoadLineMesh(const Vec3& start, const Vec3& end)
{
	wstring key = L"Line" + to_wstring(start.x) + L"_" + to_wstring(start.y) + L"_" + to_wstring(start.z) + L"_" +
		to_wstring(end.x) + L"_" + to_wstring(end.y) + L"_" + to_wstring(end.z);

	shared_ptr<Mesh> findMesh = Get<Mesh>(key);
	if (findMesh)
		return findMesh;

	vector<Vertex> vec(2);
	vec[0] = Vertex(start, Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[1] = Vertex(end, Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));

	vector<uint32> idx(2);
	idx[0] = 0;
	idx[1] = 1;

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Create(vec, idx);
	Add(key, mesh);

	return mesh;
}

shared_ptr<Mesh> Resources::LoadRectangleMesh()
{
	shared_ptr<Mesh> findMesh = Get<Mesh>(L"Rectangle");
	if (findMesh)
		return findMesh;

	float w2 = 0.5f;
	float h2 = 0.5f;

	vector<Vertex> vec(4);

	// 앞면
	vec[0] = Vertex(Vec3(-w2, -h2, 0), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[1] = Vertex(Vec3(-w2, +h2, 0), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[2] = Vertex(Vec3(+w2, +h2, 0), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[3] = Vertex(Vec3(+w2, -h2, 0), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));

	vector<uint32> idx(6);

	// 앞면
	idx[0] = 0; idx[1] = 1; idx[2] = 2;
	idx[3] = 0; idx[4] = 2; idx[5] = 3;

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Create(vec, idx);
	Add(L"Rectangle", mesh);

	return mesh;
}

shared_ptr<Mesh> Resources::LoadRectangleMesh(float width, float height, float sliceX, float sliceY)
{
	wstring key = L"Rectangle" + to_wstring(width) + L"_" + to_wstring(height) + L"_" + to_wstring(sliceX) + L"_" + to_wstring(sliceY);

	shared_ptr<Mesh> findMesh = Get<Mesh>(key);
	if (findMesh)
		return findMesh;

	float w2 = width * 0.5f;
	float h2 = height * 0.5f;

	vector<Vertex> vec;

	for (int32 y = 0; y < sliceY + 1; y++)
	{
		for (int32 x = 0; x < sliceX + 1; x++)
		{
			Vertex vtx;
			vtx.pos = Vec3(width * x / sliceX - w2, height * y / sliceY - h2, 0);
			vtx.uv = Vec2(static_cast<float>(x) / sliceX, 1.f - static_cast<float>(y) / sliceY);
			vtx.normal = Vec3(0.0f, 0.0f, -1.0f);
			vtx.tangent = Vec3(1.0f, 0.0f, 0.0f);

			vec.push_back(vtx);
		}
	}

	vector<uint32> idx;

	for (int32 y = 0; y < sliceY; y++)
	{
		for (int32 x = 0; x < sliceX; x++)
		{
			//  [0]
			//   |	\
			//  [2] - [1]
			idx.push_back((sliceX + 1) * (y + 1) + (x));
			idx.push_back((sliceX + 1) * (y)+(x + 1));
			idx.push_back((sliceX + 1) * (y)+(x));
			//  [1] - [2]
			//   	\  |
			//		  [0]
			idx.push_back((sliceX + 1) * (y)+(x + 1));
			idx.push_back((sliceX + 1) * (y + 1) + (x));
			idx.push_back((sliceX + 1) * (y + 1) + (x + 1));
		}
	}

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Create(vec, idx);
	Add(key, mesh);

	return mesh;
}

shared_ptr<Mesh> Resources::LoadCubeMesh()
{
	shared_ptr<Mesh> findMesh = Get<Mesh>(L"Cube");
	if (findMesh)
		return findMesh;

	float w2 = 0.5f;
	float h2 = 0.5f;
	float d2 = 0.5f;

	vector<Vertex> vec(24);

	// 앞면
	vec[0] = Vertex(Vec3(-w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[1] = Vertex(Vec3(-w2, +h2, -d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[2] = Vertex(Vec3(+w2, +h2, -d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[3] = Vertex(Vec3(+w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	// 뒷면
	vec[4] = Vertex(Vec3(-w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[5] = Vertex(Vec3(+w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[6] = Vertex(Vec3(+w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[7] = Vertex(Vec3(-w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	// 윗면
	vec[8] = Vertex(Vec3(-w2, +h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[9] = Vertex(Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[10] = Vertex(Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[11] = Vertex(Vec3(+w2, +h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	// 아랫면
	vec[12] = Vertex(Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[13] = Vertex(Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[14] = Vertex(Vec3(+w2, -h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[15] = Vertex(Vec3(-w2, -h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	// 왼쪽면
	vec[16] = Vertex(Vec3(-w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	vec[17] = Vertex(Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	vec[18] = Vertex(Vec3(-w2, +h2, -d2), Vec2(1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	vec[19] = Vertex(Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	// 오른쪽면
	vec[20] = Vertex(Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	vec[21] = Vertex(Vec3(+w2, +h2, -d2), Vec2(0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	vec[22] = Vertex(Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	vec[23] = Vertex(Vec3(+w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));

	vector<uint32> idx(36);

	// 앞면
	idx[0] = 0; idx[1] = 1; idx[2] = 2;
	idx[3] = 0; idx[4] = 2; idx[5] = 3;
	// 뒷면
	idx[6] = 4; idx[7] = 5; idx[8] = 6;
	idx[9] = 4; idx[10] = 6; idx[11] = 7;
	// 윗면
	idx[12] = 8; idx[13] = 9; idx[14] = 10;
	idx[15] = 8; idx[16] = 10; idx[17] = 11;
	// 아랫면
	idx[18] = 12; idx[19] = 13; idx[20] = 14;
	idx[21] = 12; idx[22] = 14; idx[23] = 15;
	// 왼쪽면
	idx[24] = 16; idx[25] = 17; idx[26] = 18;
	idx[27] = 16; idx[28] = 18; idx[29] = 19;
	// 오른쪽면
	idx[30] = 20; idx[31] = 21; idx[32] = 22;
	idx[33] = 20; idx[34] = 22; idx[35] = 23;

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Create(vec, idx);
	Add(L"Cube", mesh);

	return mesh;
}

shared_ptr<Mesh> Resources::LoadCubeMesh(float width, float height, float depth)
{
	wstring key = L"Cube" + to_wstring(width) + L"_" + to_wstring(height) + L"_" + to_wstring(depth);

	shared_ptr<Mesh> findMesh = Get<Mesh>(key);
	if (findMesh)
		return findMesh;

	float w2 = width * 0.5f;
	float h2 = height * 0.5f;
	float d2 = depth * 0.5f;

	vector<Vertex> vec(24);

	// 앞면
	vec[0] = Vertex(Vec3(-w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[1] = Vertex(Vec3(-w2, +h2, -d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[2] = Vertex(Vec3(+w2, +h2, -d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[3] = Vertex(Vec3(+w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	// 뒷면
	vec[4] = Vertex(Vec3(-w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[5] = Vertex(Vec3(+w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[6] = Vertex(Vec3(+w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[7] = Vertex(Vec3(-w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	// 윗면
	vec[8] = Vertex(Vec3(-w2, +h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[9] = Vertex(Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[10] = Vertex(Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[11] = Vertex(Vec3(+w2, +h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	// 아랫면
	vec[12] = Vertex(Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[13] = Vertex(Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[14] = Vertex(Vec3(+w2, -h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[15] = Vertex(Vec3(-w2, -h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	// 왼쪽면
	vec[16] = Vertex(Vec3(-w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	vec[17] = Vertex(Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	vec[18] = Vertex(Vec3(-w2, +h2, -d2), Vec2(1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	vec[19] = Vertex(Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	// 오른쪽면
	vec[20] = Vertex(Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	vec[21] = Vertex(Vec3(+w2, +h2, -d2), Vec2(0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	vec[22] = Vertex(Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	vec[23] = Vertex(Vec3(+w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));

	vector<uint32> idx(36);

	// 앞면
	idx[0] = 0; idx[1] = 1; idx[2] = 2;
	idx[3] = 0; idx[4] = 2; idx[5] = 3;
	// 뒷면
	idx[6] = 4; idx[7] = 5; idx[8] = 6;
	idx[9] = 4; idx[10] = 6; idx[11] = 7;
	// 윗면
	idx[12] = 8; idx[13] = 9; idx[14] = 10;
	idx[15] = 8; idx[16] = 10; idx[17] = 11;
	// 아랫면
	idx[18] = 12; idx[19] = 13; idx[20] = 14;
	idx[21] = 12; idx[22] = 14; idx[23] = 15;
	// 왼쪽면
	idx[24] = 16; idx[25] = 17; idx[26] = 18;
	idx[27] = 16; idx[28] = 18; idx[29] = 19;
	// 오른쪽면
	idx[30] = 20; idx[31] = 21; idx[32] = 22;
	idx[33] = 20; idx[34] = 22; idx[35] = 23;

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Create(vec, idx);
	Add(key, mesh);

	return mesh;
}

shared_ptr<Mesh> Resources::LoadSphereMesh()
{
	shared_ptr<Mesh> findMesh = Get<Mesh>(L"Sphere");
	if (findMesh)
		return findMesh;

	float radius = 0.5f; // 구의 반지름
	uint32 stackCount = 20; // 가로 분할
	uint32 sliceCount = 20; // 세로 분할

	vector<Vertex> vec;

	Vertex v;

	// 북극
	v.pos = Vec3(0.0f, radius, 0.0f);
	v.uv = Vec2(0.5f, 0.0f);
	v.normal = v.pos;
	v.normal.Normalize();
	v.tangent = Vec3(1.0f, 0.0f, 1.0f);
	vec.push_back(v);

	float stackAngle = XM_PI / stackCount;
	float sliceAngle = XM_2PI / sliceCount;

	float deltaU = 1.f / static_cast<float>(sliceCount);
	float deltaV = 1.f / static_cast<float>(stackCount);

	// 고리마다 돌면서 정점을 계산한다 (북극/남극 단일점은 고리가 X)
	for (uint32 y = 1; y <= stackCount - 1; ++y)
	{
		float phi = y * stackAngle;

		// 고리에 위치한 정점
		for (uint32 x = 0; x <= sliceCount; ++x)
		{
			float theta = x * sliceAngle;

			v.pos.x = radius * sinf(phi) * cosf(theta);
			v.pos.y = radius * cosf(phi);
			v.pos.z = radius * sinf(phi) * sinf(theta);

			v.uv = Vec2(deltaU * x, deltaV * y);

			v.normal = v.pos;
			v.normal.Normalize();

			v.tangent.x = -radius * sinf(phi) * sinf(theta);
			v.tangent.y = 0.0f;
			v.tangent.z = radius * sinf(phi) * cosf(theta);
			v.tangent.Normalize();

			vec.push_back(v);
		}
	}

	// 남극
	v.pos = Vec3(0.0f, -radius, 0.0f);
	v.uv = Vec2(0.5f, 1.0f);
	v.normal = v.pos;
	v.normal.Normalize();
	v.tangent = Vec3(1.0f, 0.0f, 0.0f);
	vec.push_back(v);

	vector<uint32> idx(36);

	// 북극 인덱스
	for (uint32 i = 0; i <= sliceCount; ++i)
	{
		//  [0]
		//   |  \
		//  [i+1]-[i+2]
		idx.push_back(0);
		idx.push_back(i + 2);
		idx.push_back(i + 1);
	}

	// 몸통 인덱스
	uint32 ringVertexCount = sliceCount + 1;
	for (uint32 y = 0; y < stackCount - 2; ++y)
	{
		for (uint32 x = 0; x < sliceCount; ++x)
		{
			//  [y, x]-[y, x+1]
			//  |		/
			//  [y+1, x]
			idx.push_back(1 + (y) * ringVertexCount + (x));
			idx.push_back(1 + (y) * ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x));
			//		 [y, x+1]
			//		 /	  |
			//  [y+1, x]-[y+1, x+1]
			idx.push_back(1 + (y + 1) * ringVertexCount + (x));
			idx.push_back(1 + (y) * ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x + 1));
		}
	}

	// 남극 인덱스
	uint32 bottomIndex = static_cast<uint32>(vec.size()) - 1;
	uint32 lastRingStartIndex = bottomIndex - ringVertexCount;
	for (uint32 i = 0; i < sliceCount; ++i)
	{
		//  [last+i]-[last+i+1]
		//  |      /
		//  [bottom]
		idx.push_back(bottomIndex);
		idx.push_back(lastRingStartIndex + i);
		idx.push_back(lastRingStartIndex + i + 1);
	}

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Create(vec, idx);
	Add(L"Sphere", mesh);

	return mesh;
}

shared_ptr<Mesh> Resources::LoadSphereMesh(int32 sliceCount, int32 stackCount, float sphereRadius)
{
	wstring key = L"Sphere" + to_wstring(sliceCount) + L"_" + to_wstring(stackCount);

	shared_ptr<Mesh> findMesh = Get<Mesh>(key);
	if (findMesh)
		return findMesh;

	float radius = sphereRadius; // 구의 반지름

	vector<Vertex> vec;

	Vertex v;

	// 북극
	v.pos = Vec3(0.0f, radius, 0.0f);
	v.uv = Vec2(0.5f, 0.0f);
	v.normal = v.pos;
	v.normal.Normalize();
	v.tangent = Vec3(1.0f, 0.0f, 1.0f);
	vec.push_back(v);

	float stackAngle = XM_PI / stackCount;
	float sliceAngle = XM_2PI / sliceCount;

	float deltaU = 1.f / static_cast<float>(sliceCount);
	float deltaV = 1.f / static_cast<float>(stackCount);

	// 고리마다 돌면서 정점을 계산한다 (북극/남극 단일점은 고리가 X)
	for (uint32 y = 1; y <= stackCount - 1; ++y)
	{
		float phi = y * stackAngle;

		// 고리에 위치한 정점
		for (uint32 x = 0; x <= sliceCount; ++x)
		{
			float theta = x * sliceAngle;

			v.pos.x = radius * sinf(phi) * cosf(theta);
			v.pos.y = radius * cosf(phi);
			v.pos.z = radius * sinf(phi) * sinf(theta);

			v.uv = Vec2(deltaU * x, deltaV * y);

			v.normal = v.pos;
			v.normal.Normalize();

			v.tangent.x = -radius * sinf(phi) * sinf(theta);
			v.tangent.y = 0.0f;
			v.tangent.z = radius * sinf(phi) * cosf(theta);
			v.tangent.Normalize();

			vec.push_back(v);
		}
	}

	// 남극
	v.pos = Vec3(0.0f, -radius, 0.0f);
	v.uv = Vec2(0.5f, 1.0f);
	v.normal = v.pos;
	v.normal.Normalize();
	v.tangent = Vec3(1.0f, 0.0f, 0.0f);
	vec.push_back(v);

	vector<uint32> idx(36);

	// 북극 인덱스
	for (uint32 i = 0; i <= sliceCount; ++i)
	{
		//  [0]
		//   |  \
		//  [i+1]-[i+2]
		idx.push_back(0);
		idx.push_back(i + 2);
		idx.push_back(i + 1);
	}

	// 몸통 인덱스
	uint32 ringVertexCount = sliceCount + 1;
	for (uint32 y = 0; y < stackCount - 2; ++y)
	{
		for (uint32 x = 0; x < sliceCount; ++x)
		{
			//  [y, x]-[y, x+1]
			//  |		/
			//  [y+1, x]
			idx.push_back(1 + (y) * ringVertexCount + (x));
			idx.push_back(1 + (y) * ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x));
			//		 [y, x+1]
			//		 /	  |
			//  [y+1, x]-[y+1, x+1]
			idx.push_back(1 + (y + 1) * ringVertexCount + (x));
			idx.push_back(1 + (y) * ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x + 1));
		}
	}

	// 남극 인덱스
	uint32 bottomIndex = static_cast<uint32>(vec.size()) - 1;
	uint32 lastRingStartIndex = bottomIndex - ringVertexCount;
	for (uint32 i = 0; i < sliceCount; ++i)
	{
		//  [last+i]-[last+i+1]
		//  |      /
		//  [bottom]
		idx.push_back(bottomIndex);
		idx.push_back(lastRingStartIndex + i);
		idx.push_back(lastRingStartIndex + i + 1);
	}

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Create(vec, idx);
	Add(key, mesh);

	return mesh;
}

shared_ptr<Mesh> Resources::LoadTerrainMesh(int32 sizeX, int32 sizeZ)
{
	vector<Vertex> vec;

	for (int32 z = 0; z < sizeZ + 1; z++)
	{
		for (int32 x = 0; x < sizeX + 1; x++)
		{
			Vertex vtx;
			vtx.pos = Vec3(static_cast<float>(x), 0, static_cast<float>(z));
			vtx.uv = Vec2(static_cast<float>(x), static_cast<float>(sizeZ - z));
			vtx.normal = Vec3(0.f, 1.f, 0.f);
			vtx.tangent = Vec3(1.f, 0.f, 0.f);

			vec.push_back(vtx);
		}
	}

	vector<uint32> idx;

	for (int32 z = 0; z < sizeZ; z++)
	{
		for (int32 x = 0; x < sizeX; x++)
		{
			//  [0]
			//   |	\
			//  [2] - [1]
			idx.push_back((sizeX + 1) * (z + 1) + (x));
			idx.push_back((sizeX + 1) * (z)+(x + 1));
			idx.push_back((sizeX + 1) * (z)+(x));
			//  [1] - [2]
			//   	\  |
			//		  [0]
			idx.push_back((sizeX + 1) * (z)+(x + 1));
			idx.push_back((sizeX + 1) * (z + 1) + (x));
			idx.push_back((sizeX + 1) * (z + 1) + (x + 1));
		}
	}

	shared_ptr<Mesh> findMesh = Get<Mesh>(L"Terrain");
	if (findMesh)
	{
		findMesh->Create(vec, idx);
		return findMesh;
	}

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Create(vec, idx);
	Add(L"Terrain", mesh);
	return mesh;
}

shared_ptr<Mesh> Resources::LoadCapsuleMesh(float radius, float height)
{
	// 캐시된 메시가 있는지 확인
	wstring key = L"Capsule_" + to_wstring(radius) + L"_" + to_wstring(height);
	shared_ptr<Mesh> findMesh = Get<Mesh>(key);
	if (findMesh)
		return findMesh;

	// 캡슐 메시 생성에 필요한 변수들
	const int32 stackCount = 20;  // 세로 분할
	const int32 sliceCount = 20;  // 가로 분할

	vector<Vertex> vertices;
	vector<uint32> indices;

	// 1. 위쪽 반구 생성
	for (int32 y = 0; y <= stackCount / 2; ++y)
	{
		float phi = XM_PI / 2.0f - y * XM_PI / stackCount;
		float yPos = radius * sin(phi);
		float r = radius * cos(phi);

		for (int32 x = 0; x <= sliceCount; ++x)
		{
			float theta = x * XM_2PI / sliceCount;

			Vertex vertex;
			vertex.pos = Vec3(r * cos(theta), yPos + height / 2.0f, r * sin(theta));
			vertex.uv = Vec2(static_cast<float>(x) / sliceCount, static_cast<float>(y) / stackCount);
            Vec3 normal = vertex.pos - Vec3(0, height / 2.0f, 0);
            normal.Normalize();
            vertex.normal = normal;
			vertex.tangent = Vec3(-sin(theta), 0.0f, cos(theta));

			vertices.push_back(vertex);
		}
	}

	// 2. 실린더 부분 생성
	for (int32 y = 1; y < stackCount; ++y)
	{
		float yPos = height / 2.0f - y * height / stackCount;

		for (int32 x = 0; x <= sliceCount; ++x)
		{
			float theta = x * XM_2PI / sliceCount;

			Vertex vertex;
			vertex.pos = Vec3(radius * cos(theta), yPos, radius * sin(theta));
			vertex.uv = Vec2(static_cast<float>(x) / sliceCount, 0.5f + yPos / height);
			vertex.normal = Vec3(cos(theta), 0.0f, sin(theta));
			vertex.tangent = Vec3(-sin(theta), 0.0f, cos(theta));

			vertices.push_back(vertex);
		}
	}

	// 3. 아래쪽 반구 생성
	for (int32 y = stackCount / 2; y <= stackCount; ++y)
	{
		float phi = XM_PI / 2.0f - y * XM_PI / stackCount;
		float yPos = radius * sin(phi);
		float r = radius * cos(phi);

		for (int32 x = 0; x <= sliceCount; ++x)
		{
			float theta = x * XM_2PI / sliceCount;

			Vertex vertex;
			vertex.pos = Vec3(r * cos(theta), yPos - height / 2.0f, r * sin(theta));
			vertex.uv = Vec2(static_cast<float>(x) / sliceCount, static_cast<float>(y) / stackCount);
			Vec3 normal = vertex.pos - Vec3(0, height / 2.0f, 0);
			normal.Normalize();
			vertex.normal = normal;
			vertex.tangent = Vec3(-sin(theta), 0.0f, cos(theta));

			vertices.push_back(vertex);
		}
	}

	// 인덱스 생성
	for (int32 y = 0; y < stackCount; ++y)
	{
		for (int32 x = 0; x < sliceCount; ++x)
		{
			indices.push_back((y + 0) * (sliceCount + 1) + (x + 0));
			indices.push_back((y + 0) * (sliceCount + 1) + (x + 1));
			indices.push_back((y + 1) * (sliceCount + 1) + (x + 0));

			indices.push_back((y + 0) * (sliceCount + 1) + (x + 1));
			indices.push_back((y + 1) * (sliceCount + 1) + (x + 1));
			indices.push_back((y + 1) * (sliceCount + 1) + (x + 0));
		}
	}

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Create(vertices, indices);
	Add(key, mesh);

	return mesh;
}

shared_ptr<Texture> Resources::CreateTexture(const wstring& name, DXGI_FORMAT format, uint32 width, uint32 height,
	const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
	D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor)
{
	shared_ptr<Texture> texture = make_shared<Texture>();
	texture->Create(format, width, height, heapProperty, heapFlags, resFlags, clearColor);
	Add(name, texture);

	return texture;
}

shared_ptr<Texture> Resources::CreateTextureFromResource(const wstring& name, ComPtr<ID3D12Resource> tex2D)
{
	shared_ptr<Texture> texture = make_shared<Texture>();
	texture->CreateFromResource(tex2D);
	Add(name, texture);

	return texture;
}

shared_ptr<MeshData> Resources::LoadFBX(const wstring& path)
{
	wstring key = path;

	shared_ptr<MeshData> meshData = Get<MeshData>(key);
	if (meshData)
		return meshData;

	meshData = MeshData::LoadFromFBX(path);
	meshData->SetName(key);
	Add(key, meshData);

	return meshData;
}

void Resources::CreateDefaultShader()
{
	// Skybox
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::LESS_EQUAL
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\skybox.fx", info);
		Add<Shader>(L"Skybox", shader);
	}

	// Deferred (Deferred)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::DEFERRED
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\deferred.fx", info);
		Add<Shader>(L"Deferred", shader);
	}

	// Forward (Forward)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\forward.fx", info);
		Add<Shader>(L"Forward", shader);
	}

	// Texture (Forward)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE
		};

		ShaderArg arg =
		{
			"VS_Tex",
			"",
			"",
			"",
			"PS_Tex"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\forward.fx", info, arg);
		Add<Shader>(L"Texture", shader);
	}

	// DirLight
	{
		ShaderInfo info =
		{
			SHADER_TYPE::LIGHTING,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ONE_TO_ONE_BLEND
		};

		ShaderArg arg =
		{
			"VS_DirLight",
			"",
			"",
			"",
			"PS_DirLight"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\lighting.fx", info, arg);
		Add<Shader>(L"DirLight", shader);
	}

	// PointLight
	{
		ShaderInfo info =
		{
			SHADER_TYPE::LIGHTING,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ONE_TO_ONE_BLEND
		};

		ShaderArg arg =
		{
			"VS_PointLight",
			"",
			"",
			"",
			"PS_PointLight"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\lighting.fx", info, arg);
		Add<Shader>(L"PointLight", shader);
	}

	// Final
	{
		ShaderInfo info =
		{
			SHADER_TYPE::LIGHTING,
			RASTERIZER_TYPE::CULL_BACK,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
		};

		ShaderArg arg =
		{
			"VS_Final",
			"",
			"",
			"",
			"PS_Final"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\lighting.fx", info, arg);
		Add<Shader>(L"Final", shader);
	}

	// Compute Shader
	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateComputeShader(L"..\\Resources\\Shader\\compute.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"ComputeShader", shader);
	}

	// Particle
	{
		ShaderInfo info =
		{
			SHADER_TYPE::PARTICLE,
			RASTERIZER_TYPE::CULL_BACK,
			DEPTH_STENCIL_TYPE::LESS_NO_WRITE,
			BLEND_TYPE::ALPHA_BLEND,
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"",
			"",
			"GS_Main",
			"PS_Main"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\particle.fx", info, arg);
		Add<Shader>(L"Particle", shader);
	}

	// ComputeParticle
	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateComputeShader(L"..\\Resources\\Shader\\particle.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"ComputeParticle", shader);
	}

	// Shadow
	{
		ShaderInfo info =
		{
			SHADER_TYPE::SHADOW,
			RASTERIZER_TYPE::CULL_BACK,
			DEPTH_STENCIL_TYPE::LESS,
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\shadow.fx", info);
		Add<Shader>(L"Shadow", shader);
	}

	// Tessellation
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::WIREFRAME,
			DEPTH_STENCIL_TYPE::LESS,
			BLEND_TYPE::DEFAULT,
			D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"HS_Main",
			"DS_Main",
			"",
			"PS_Main",
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\tessellation.fx", info, arg);
		Add<Shader>(L"Tessellation", shader);
	}

	// Terrain
	{
		ShaderInfo info =
		{
			SHADER_TYPE::DEFERRED,
			RASTERIZER_TYPE::CULL_BACK,
			DEPTH_STENCIL_TYPE::LESS,
			BLEND_TYPE::DEFAULT,
			D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"HS_Main",
			"DS_Main",
			"",
			"PS_Main",
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\terrain.fx", info, arg);
		Add<Shader>(L"Terrain", shader);
	}

	// ComputeAnimation
	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateComputeShader(L"..\\Resources\\Shader\\animation.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"ComputeAnimation", shader);
	}

	// DebugVisualization
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::WIREFRAME,
			DEPTH_STENCIL_TYPE::LESS,
			BLEND_TYPE::DEFAULT,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"",
			"",
			"",
			"PS_Main"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\debugVisualization.fx", info);
		Add<Shader>(L"DebugVisualization", shader);
	}

	// DebugVisualization(Line)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::WIREFRAME,
			DEPTH_STENCIL_TYPE::LESS,
			BLEND_TYPE::DEFAULT,
			D3D_PRIMITIVE_TOPOLOGY_LINESTRIP
		};

		ShaderArg arg =
		{
			"VS_Main",
			"",
			"",
			"",
			"PS_Main"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\debugVisualization.fx", info);
		Add<Shader>(L"LineDebugVisualization", shader);
	}

	// Laser
	{
		ShaderInfo info = 
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::LESS,
			BLEND_TYPE::ALPHA_BLEND,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\laser.fx", info);
		Add<Shader>(L"Laser", shader);
	}

	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\muzzle_flash.fx",
			{ SHADER_TYPE::FORWARD, RASTERIZER_TYPE::CULL_NONE, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE, BLEND_TYPE::ALPHA_BLEND });
		Add<Shader>(L"MuzzleFlash", shader);
	}

	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\shockwave.fx",
			{ SHADER_TYPE::FORWARD, RASTERIZER_TYPE::CULL_NONE, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE, BLEND_TYPE::ALPHA_BLEND });
		Add<Shader>(L"Shockwave", shader);
	}

	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\collision_effect.fx",
			{ SHADER_TYPE::FORWARD, RASTERIZER_TYPE::CULL_NONE, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE, BLEND_TYPE::ALPHA_BLEND });
		Add<Shader>(L"CollisionEffect", shader);
	}

	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::LESS,
			BLEND_TYPE::ALPHA_BLEND,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\fadeout.fx", info);
		Add<Shader>(L"FadeOut", shader);
	}

	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\breath_effect.fx",
			{ SHADER_TYPE::FORWARD, RASTERIZER_TYPE::CULL_NONE, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE, BLEND_TYPE::ALPHA_BLEND });
		Add<Shader>(L"BreathEffect", shader);
	}

	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\swing_effect.fx",
			{ SHADER_TYPE::FORWARD, RASTERIZER_TYPE::CULL_NONE, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE, BLEND_TYPE::ALPHA_BLEND });
		Add<Shader>(L"SwingEffect", shader);
	}

	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\smash_effect.fx",
			{ SHADER_TYPE::FORWARD, RASTERIZER_TYPE::CULL_NONE, DEPTH_STENCIL_TYPE::LESS, BLEND_TYPE::ALPHA_BLEND });
		Add<Shader>(L"SmashEffect", shader);
	}

	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\lava.fx",
			{ SHADER_TYPE::FORWARD, RASTERIZER_TYPE::CULL_NONE, DEPTH_STENCIL_TYPE::LESS, BLEND_TYPE::DEFAULT });
		Add<Shader>(L"Lava", shader);
	}

	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\flame.fx",
			{ SHADER_TYPE::FORWARD, RASTERIZER_TYPE::CULL_NONE, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE, BLEND_TYPE::ALPHA_BLEND });
		Add<Shader>(L"Flame", shader);
	}

	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\smoke.fx",
			{ SHADER_TYPE::FORWARD, RASTERIZER_TYPE::CULL_NONE, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE, BLEND_TYPE::ALPHA_BLEND });
		Add<Shader>(L"Smoke", shader);
	}
}

void Resources::CreateDefaultMaterial()
{
	// Skybox
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Skybox");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"Skybox", material);
	}

	// DirLight
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"DirLight");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, GET_SINGLE(Resources)->Get<Texture>(L"PositionTarget"));
		material->SetTexture(1, GET_SINGLE(Resources)->Get<Texture>(L"NormalTarget"));
		Add<Material>(L"DirLight", material);
	}

	// PointLight
	{
		const WindowInfo& window = GEngine->GetWindow();
		Vec2 resolution = { static_cast<float>(window.width), static_cast<float>(window.height) };

		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"PointLight");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, GET_SINGLE(Resources)->Get<Texture>(L"PositionTarget"));
		material->SetTexture(1, GET_SINGLE(Resources)->Get<Texture>(L"NormalTarget"));
		material->SetVec2(0, resolution);
		Add<Material>(L"PointLight", material);
	}

	// Final
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Final");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, GET_SINGLE(Resources)->Get<Texture>(L"DiffuseTarget"));
		material->SetTexture(1, GET_SINGLE(Resources)->Get<Texture>(L"DiffuseLightTarget"));
		material->SetTexture(2, GET_SINGLE(Resources)->Get<Texture>(L"SpecularLightTarget"));
		Add<Material>(L"Final", material);
	}

	// Compute Shader
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeShader");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"ComputeShader", material);
	}

	// Particle
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Particle");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"Particle", material);
	}

	// ComputeParticle
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeParticle");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);

		Add<Material>(L"ComputeParticle", material);
	}

	// GameObject
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Deferred");
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Leather", L"..\\Resources\\Texture\\Leather.jpg");
		shared_ptr<Texture> texture2 = GET_SINGLE(Resources)->Load<Texture>(L"Leather_Normal", L"..\\Resources\\Texture\\Leather_Normal.jpg");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		material->SetTexture(1, texture2);
		Add<Material>(L"GameObject", material);
	}

	// Shadow
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Shadow");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"Shadow", material);
	}

	// Tessellation
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Tessellation");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"Tessellation", material);
	}

	// Terrain
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Terrain");
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Terrain", L"..\\Resources\\Texture\\Terrain\\terrain.png");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		Add<Material>(L"Terrain", material);
	}

	// ComputeAnimation
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeAnimation");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);

		Add<Material>(L"ComputeAnimation", material);
	}

	// DebugVisualization
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"DebugVisualization");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"DebugVisualization", material);
	}

	// DebugVisualization(Line)
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"LineDebugVisualization");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"LineDebugVisualization", material);
	}

	// Laser Material
	{
		// uniform float4 g_vec4_0; // 기본 레이저 색상
		// uniform float4 g_vec4_1; // 보조(하이라이트) 레이저 색상
		// uniform float2 g_vec2_0.x; // 블렌딩 속도
		// uniform float  g_float_0; // 시간
		// uniform float  g_float_1; // 발광 강도
		// uniform float  g_float_2; // 왜곡(디스토션) 강도
		// uniform float  g_float_3; // 왜곡 주파수

		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Laser");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetName(L"Laser");

		material->SetVec4(0, Vec4(0.1f, 0.3f, 1.0f, 0.7f));  // 기본 색상
		material->SetVec4(1, Vec4(0.0f, 0.2f, 1.0f, 1.0f));  // 보조 색상
		material->SetVec2(0, Vec2(20.0f, 0.0f));  // 블렌딩 속도
		material->SetFloat(0, 0.0f);  // 시간값 (업데이트 필요)
		material->SetFloat(1, 5.0f);  // 발광 강도
		material->SetFloat(2, 0.1f);  // 왜곡 강도
		material->SetFloat(3, 10.0f);  // 왜곡 주파수

		Add<Material>(L"Laser", material);
	}

	{
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(
			L"MuzzleFlashTex", L"..\\Resources\\Texture\\muzzle_flash.png");

		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(Get<Shader>(L"MuzzleFlash"));
		material->SetTexture(0, texture);

		Add<Material>(L"MuzzleFlash", material);
	}

	{
		// 텍스처 설정
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(
			L"ShockwaveTex", L"..\\Resources\\Texture\\shockwave.png");

		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(Get<Shader>(L"Shockwave"));
		material->SetTexture(0, texture);

		Add<Material>(L"Shockwave", material);
	}

	{
		// 텍스처 설정
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(
			L"CollisionEffectTex", L"..\\Resources\\Texture\\collision_effect.png");

		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(Get<Shader>(L"CollisionEffect"));
		material->SetTexture(0, texture);
		Add<Material>(L"CollisionEffect", material);
	}

	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"FadeOut");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetName(L"FadeOut");

		Add<Material>(L"FadeOut", material);
	}

	{
		auto noiseTexture = GET_SINGLE(Resources)->Load<Texture>(
			L"NoiseTexture", L"..\\Resources\\Texture\\noise2.png");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(Get<Shader>(L"BreathEffect"));
		material->SetVec4(1, Vec4(1.0f, 0.65f, 0.0f, 1.0f));
		material->SetVec4(2, Vec4(1.0f, 1.0f, 0.88f, 1.0f));
		// turbulence intensity, swirl speed, flicker intensity
		material->SetVec4(3, Vec4(1.0f, 0.5f, 1.2f, 0.0f));
		material->SetTexture(0, noiseTexture);

		Add<Material>(L"BreathEffect", material);
	}

	{
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(
			L"SwingEffectTex", L"..\\Resources\\Texture\\claw.png");

		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(Get<Shader>(L"SwingEffect"));
		material->SetTexture(0, texture);

		Add<Material>(L"SwingEffect", material);
	}

	{
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(
			L"SmashEffectTex", L"..\\Resources\\Texture\\boss_shockwave.png");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(Get<Shader>(L"SmashEffect"));
		material->SetTexture(0, texture);
		Add<Material>(L"SmashEffect", material);
	}

	{
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(Get<Shader>(L"Lava"));
		Add<Material>(L"Lava", material);
	}

	{
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(Get<Shader>(L"Flame"));
		Add<Material>(L"Flame", material);
	}	

	{
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(Get<Shader>(L"Smoke"));
		Add<Material>(L"Smoke", material);
	}
}

void Resources::CreateDefaultSound()
{
	// 기본 배경음악 로드
	{
		shared_ptr<Sound> sound = Load<Sound>(
			L"MainStageBGM", L"..\\Resources\\Sound\\Background\\main_stage_bgm.wav");
		sound->SetVolume(1.0f);
		sound->SetLoop(true);
		Add(L"MainStageBGM", sound);
	}

	// 보스 배경음악 로드
	{
		shared_ptr<Sound> sound = Load<Sound>(
			L"BossStageBGM", L"..\\Resources\\Sound\\Background\\boss_stage_bgm.wav");
		sound->SetVolume(0.1f);
		sound->SetLoop(true);
		Add(L"BossStageBGM", sound);
	}

	// 레이저 샷
	{
		shared_ptr<Sound> sound = Load<Sound>(
			L"LaserShot", L"..\\Resources\\Sound\\FX\\laser_shot.mp3");
		sound->SetVolume(10.f);
		Add(L"LaserShot", sound);
	}

	// 레이저 히트
	{
		shared_ptr<Sound> sound = Load<Sound>(
			L"LaserHit", L"..\\Resources\\Sound\\FX\\laser_hit.mp3");
		sound->SetVolume(50.f);
		Add(L"LaserHit", sound);
	}

	// Footstep
	{
		shared_ptr<Sound> sound = Load<Sound>(
			L"Footsteps", L"..\\Resources\\Sound\\FX\\footsteps.mp3");
		Add(L"Footsteps", sound);
	}

	// PlayerHit
	{
		shared_ptr<Sound> sound = Load<Sound>(
			L"PlayerHit", L"..\\Resources\\Sound\\FX\\player_hit.mp3");
		sound->SetVolume(50.f);
		Add(L"PlayerHit", sound);
	}

	// BossSwing
	{
		shared_ptr<Sound> sound = Load<Sound>(
			L"BossSwing", L"..\\Resources\\Sound\\FX\\boss_swing.mp3");
		sound->SetVolume(50.f);
		Add(L"BossSwing", sound);
	}

	// BossSmash
	{
		shared_ptr<Sound> sound = Load<Sound>(
			L"BossSmash", L"..\\Resources\\Sound\\FX\\boss_smash.mp3");
		sound->SetVolume(50.f);
		Add(L"BossSmash", sound);
	}

	// BossExplosion
	{
		shared_ptr<Sound> sound = Load<Sound>(
			L"BossExplosion", L"..\\Resources\\Sound\\FX\\boss_explosion.mp3");
		sound->SetVolume(50.f);
		Add(L"BossExplosion", sound);
	}

	// BossRoar
	{
		shared_ptr<Sound> sound = Load<Sound>(
			L"BossRoar", L"..\\Resources\\Sound\\FX\\boss_roar.mp3");
		sound->SetVolume(50.f);
		Add(L"BossRoar", sound);
	}

	// BossBreath
	{
		shared_ptr<Sound> sound = Load<Sound>(
			L"BossBreath", L"..\\Resources\\Sound\\FX\\boss_breath.mp3");
		sound->SetVolume(50.f);
		Add(L"BossBreath", sound);
	}

	// BossDead
	{
		shared_ptr<Sound> sound = Load<Sound>(
			L"BossDead", L"..\\Resources\\Sound\\FX\\boss_dead.mp3");
		sound->SetVolume(70.f);
		Add(L"BossDead", sound);
	}

	// BossCharge
	{
		shared_ptr<Sound> sound = Load<Sound>(
			L"BossCharge", L"..\\Resources\\Sound\\FX\\boss_charge.mp3");
		sound->SetVolume(50.f);
		Add(L"BossCharge", sound);
	}

	// KeyObtain
	{
		shared_ptr<Sound> sound = Load<Sound>(
			L"KeyObtain", L"..\\Resources\\Sound\\FX\\key_obtain.mp3");
		sound->SetVolume(10.f);
		Add(L"KeyObtain", sound);
	}

	// Lava
	{
		shared_ptr<Sound> sound = Load<Sound>(
			L"Lava", L"..\\Resources\\Sound\\FX\\lava-loop-3-28887.mp3");
		sound->SetVolume(50.f);
		sound->SetLoop(true);
		Add(L"Lava", sound);
	}
}
