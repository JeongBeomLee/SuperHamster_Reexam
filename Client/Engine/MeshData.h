#pragma once
#include "Object.h"

class Mesh;
class Material;
class GameObject;

struct MeshRenderInfo
{
	shared_ptr<Mesh>				mesh;
	vector<shared_ptr<Material>>	materials;
};

class MeshData : public Object
{
public:
	MeshData();
	virtual ~MeshData();

public:
	static shared_ptr<MeshData> LoadFromFBX(const wstring& path);

	virtual void Load(const wstring& path);
	virtual void Save(const wstring& path);

	void CountMeshMaterialPairs();
	vector<shared_ptr<GameObject>> Instantiate();

	vector<MeshRenderInfo>& GetMeshRenders() { return _meshRenders; }

private:
	vector<MeshRenderInfo> _meshRenders;

	map<pair<uint64, uint64>, int> _meshMaterialCount;
};
