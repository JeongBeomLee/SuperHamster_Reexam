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

		// Material 찾아서 연동
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
	// 메시-머터리얼 페어 카운트 업데이트
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

			// 같은 메시-머터리얼 페어가 2개 이상 사용되면 인스턴싱 활성화
			if (_meshMaterialCount[{info.mesh->GetID(), info.materials[i]->GetID()}] >= 2)
			{
				//material = info.materials[i];
				//material->SetInt(0, 1);  // 인스턴싱 활성화
				material = info.materials[i]->Clone();
				material->SetInt(0, 0);  // 인스턴싱 비활성화
			}
			else
			{
				material = info.materials[i]->Clone();
				material->SetInt(0, 0);  // 인스턴싱 비활성화
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

			// 기본적으로 첫 번째 애니메이션 재생 시작
			transformAnim->Play(0);
			transformAnim->SetLoop(true);

			// 디버그 로그
			Logger::Instance().Debug("트랜스폼 애니메이션 추가됨: GameObject [{}]",
				ws2s(gameObject->GetName()));
		}

		v.push_back(gameObject);
	}
	return v;
}