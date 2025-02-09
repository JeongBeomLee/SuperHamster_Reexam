#include "pch.h"
#include "MeshData.h"
#include "FBXLoader.h"
#include "Mesh.h"
#include "Material.h"
#include "Resources.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Animator.h"
#include "TransformAnimator.h"

MeshData::MeshData() : Object(OBJECT_TYPE::MESH_DATA)
{
}

MeshData::~MeshData()
{
}

shared_ptr<MeshData> MeshData::LoadFromFBX(const wstring& path)
{
	FBXLoader loader;
	loader.LoadFbx(path);

	shared_ptr<MeshData> meshData = make_shared<MeshData>();

	for (int32 i = 0; i < loader.GetMeshCount(); i++)
	{
		uint32 vertexCount = loader.GetMesh(i).vertices.size();
		uint32 indexCount = loader.GetMesh(i).indices.size() > 0 ? loader.GetMesh(i).indices[0].size() : 0;

		shared_ptr<Mesh> mesh = Mesh::CreateFromFBX(&loader.GetMesh(i), loader);
		mesh->SetFbxMeshInfo(loader.GetMesh(i));

		GET_SINGLE(Resources)->Add<Mesh>(mesh->GetName(), mesh);

		// Material ã�Ƽ� ����
		vector<shared_ptr<Material>> materials;
		for (size_t j = 0; j < loader.GetMesh(i).materials.size(); j++)
		{
			shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(loader.GetMesh(i).materials[j].name);
			materials.push_back(material);
		}

		MeshRenderInfo info = {};
		info.mesh = mesh;
		info.materials = materials;
		meshData->_meshRenders.push_back(info);
	}

	return meshData;
}

void MeshData::Load(const wstring& _strFilePath)
{
	// TODO
}

void MeshData::Save(const wstring& _strFilePath)
{
	// TODO
}

void MeshData::CountMeshMaterialPairs()
{
	_meshMaterialCount.clear();
	for (const auto& info : _meshRenders)
	{
		for (const auto& material : info.materials)
		{
			_meshMaterialCount[{info.mesh->GetID(), material->GetID()}]++;
		}
	}
}

vector<shared_ptr<GameObject>> MeshData::Instantiate()
{
	// �޽�-���͸��� ��� ī��Ʈ ������Ʈ
	CountMeshMaterialPairs();

	vector<shared_ptr<GameObject>> v;
	for (MeshRenderInfo& info : _meshRenders)
	{
		shared_ptr<GameObject> gameObject = make_shared<GameObject>();
		gameObject->SetName(info.mesh->GetName());
		gameObject->AddComponent(make_shared<Transform>());
		gameObject->AddComponent(make_shared<MeshRenderer>());
		gameObject->GetMeshRenderer()->SetMesh(info.mesh);

		for (uint32 i = 0; i < info.materials.size(); i++)
		{
			shared_ptr<Material> material;

			// ���� �޽�-���͸��� �� 2�� �̻� ���Ǹ� �ν��Ͻ� Ȱ��ȭ
			if (_meshMaterialCount[{info.mesh->GetID(), info.materials[i]->GetID()}] >= 2)
			{
				//material = info.materials[i];
				//material->SetInt(0, 1);  // �ν��Ͻ� Ȱ��ȭ
				material = info.materials[i]->Clone();
				material->SetInt(0, 0);  // �ν��Ͻ� ��Ȱ��ȭ
			}
			else
			{
				material = info.materials[i]->Clone();
				material->SetInt(0, 0);  // �ν��Ͻ� ��Ȱ��ȭ
			}

			gameObject->GetMeshRenderer()->SetMaterial(material, i);
		}

		if (info.mesh->IsAnimMesh())
		{
			shared_ptr<Animator> animator = make_shared<Animator>();
			gameObject->AddComponent(animator);
			animator->SetBones(info.mesh->GetBones());
			animator->SetAnimClip(info.mesh->GetAnimClip());
		}

		if (info.mesh->GetFbxMeshInfo().hasTransformAnimation)
		{
			shared_ptr<TransformAnimator> transformAnim = make_shared<TransformAnimator>();
			gameObject->AddComponent(transformAnim);
			transformAnim->SetTransformAnimClip(&info.mesh->GetFbxMeshInfo().transformAnimClips);

			// �⺻������ ù ��° �ִϸ��̼� ��� ����
			transformAnim->Play(0);
			transformAnim->SetLoop(true);

			// ����� �α�
			Logger::Instance().Debug("Ʈ������ �ִϸ��̼� �߰���: GameObject [{}]",
				ws2s(gameObject->GetName()));
		}

		v.push_back(gameObject);
	}
	return v;
}