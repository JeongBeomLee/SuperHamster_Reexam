#pragma once

struct FbxMaterialInfo
{
    Vec4            diffuse;
    Vec4            ambient;
    Vec4            specular;
    wstring         name;
    wstring         diffuseTexName;
    wstring         normalTexName;
    wstring         specularTexName;
};

struct BoneWeight
{
    using Pair = pair<int32_t, double>;
    vector<Pair> boneWeights;

    void AddWeights(uint32_t index, double weight)
    {
        if (weight <= 0.0) // Corrected comparison with double literal
            return;

        auto findIt = std::find_if(boneWeights.begin(), boneWeights.end(),
            [weight](const Pair& p) { return p.second < weight; }); // Removed '=' from lambda capture

        if (findIt != boneWeights.end())
            boneWeights.insert(findIt, Pair(index, weight));
        else
            boneWeights.push_back(Pair(index, weight));

        // Limit weights to a maximum of 4
        if (boneWeights.size() > 4)
            boneWeights.pop_back();
    }

    void Normalize()
    {
        double sum = 0.f;
        std::for_each(boneWeights.begin(), boneWeights.end(), [&](Pair& p) { sum += p.second; });
        std::for_each(boneWeights.begin(), boneWeights.end(), [=](Pair& p) { p.second = p.second / sum; });
    }
};

struct FbxMeshInfo
{
    wstring name;
    vector<Vertex> vertices;
    vector<vector<uint32_t>> indices;
    vector<FbxMaterialInfo> materials;
    vector<BoneWeight> boneWeights; // Bone weights
    bool hasAnimation;

    // 트랜스폼 정보 추가
    Matrix transform;         // 로컬 변환 행렬
    Matrix globalTransform;   // 글로벌 변환 행렬

    Vec3 GetPosition() const
	{
		return { transform.m[3][0], transform.m[3][1], transform.m[3][2] };
	}

    Vec3 GetScale() const
    {
        float scaleX = sqrt(transform.m[0][0] * transform.m[0][0] + transform.m[0][1] * transform.m[0][1] + transform.m[0][2] * transform.m[0][2]);
        float scaleY = sqrt(transform.m[1][0] * transform.m[1][0] + transform.m[1][1] * transform.m[1][1] + transform.m[1][2] * transform.m[1][2]);
        float scaleZ = sqrt(transform.m[2][0] * transform.m[2][0] + transform.m[2][1] * transform.m[2][1] + transform.m[2][2] * transform.m[2][2]);
        return { scaleX, scaleY, scaleZ };
    }

    Vec3 GetRotation() const
	{
		Matrix matRotation = transform;

		// 회전 행렬을 사용하여 회전 각도 계산
		float pitch = atan2(-matRotation.m[2][1], matRotation.m[2][2]);
		float yaw = atan2(matRotation.m[2][0], sqrt(matRotation.m[2][1] * matRotation.m[2][1] + matRotation.m[2][2] * matRotation.m[2][2]));
		float roll = atan2(-matRotation.m[1][0], matRotation.m[0][0]);

		return { pitch, yaw, roll };
	}

    Vec3 GetGlobalPosition() const
    {
        return { globalTransform.m[3][0], globalTransform.m[3][1], globalTransform.m[3][2] };
    }

    Vec3 GetGlobalScale() const
    {
        float scaleX = sqrt(globalTransform.m[0][0] * globalTransform.m[0][0] + globalTransform.m[0][1] * globalTransform.m[0][1] + globalTransform.m[0][2] * globalTransform.m[0][2]);
        float scaleY = sqrt(globalTransform.m[1][0] * globalTransform.m[1][0] + globalTransform.m[1][1] * globalTransform.m[1][1] + globalTransform.m[1][2] * globalTransform.m[1][2]);
        float scaleZ = sqrt(globalTransform.m[2][0] * globalTransform.m[2][0] + globalTransform.m[2][1] * globalTransform.m[2][1] + globalTransform.m[2][2] * globalTransform.m[2][2]);

        return { scaleX, scaleY, scaleZ };
    }

    Vec3 GetGlobalRotation() const
    {
        Matrix matRotation = globalTransform;

        // 회전 행렬을 사용하여 회전 각도 계산
        float pitch = atan2(-matRotation.m[2][1], matRotation.m[2][2]);
        float yaw = atan2(matRotation.m[2][0], sqrt(matRotation.m[2][1] * matRotation.m[2][1] + matRotation.m[2][2] * matRotation.m[2][2]));
        float roll = atan2(-matRotation.m[1][0], matRotation.m[0][0]);

        return { pitch, yaw, roll };
    }
};

struct FbxKeyFrameInfo
{
    FbxAMatrix  matTransform;
    double      time;
};

struct FbxBoneInfo
{
    wstring                 boneName;
    int32_t                 parentIndex;
    FbxAMatrix              matOffset;
};

struct FbxAnimClipInfo
{
    wstring                             name;
    FbxTime                             startTime;
    FbxTime                             endTime;
    FbxTime::EMode                      mode;
    vector<vector<FbxKeyFrameInfo>>     keyFrames;
};

class FBXLoader
{
public:
    FBXLoader();
    ~FBXLoader();

    void LoadFbx(const wstring& path);

    int32_t GetMeshCount() const { return static_cast<int32_t>(_meshes.size()); } // Added const correctness
    FbxMeshInfo& GetMesh(int32_t idx) { return _meshes[idx]; } // Added const correctness
    vector<shared_ptr<FbxBoneInfo>>& GetBones() { return _bones; }
    vector<shared_ptr<FbxAnimClipInfo>>& GetAnimClip() { return _animClips; }
    wstring GetResourceDirectory() const { return _resourceDirectory; } // Added const correctness

private:
    void Import(const wstring& path);

    void ParseNode(FbxNode* root);
    void LoadMesh(FbxMesh* mesh, FbxMeshInfo* meshInfo);
    void LoadMaterial(FbxSurfaceMaterial* surfaceMaterial);

    void        GetNormal(FbxMesh* mesh, FbxMeshInfo* container, int32_t idx, int32_t vertexCounter);
    void        GetTangent(FbxMesh* mesh, FbxMeshInfo* container, int32_t idx, int32_t vertexCounter);
    void        GetUV(FbxMesh* mesh, FbxMeshInfo* container, int32_t idx, int32_t vertexCounter);
    Vec4        GetMaterialData(FbxSurfaceMaterial* surface, const char* materialName, const char* factorName);
    wstring     GetTextureRelativeName(FbxSurfaceMaterial* surface, const char* materialProperty);

    void CreateTextures();
    void CreateMaterials();

    // Animation
    void LoadBones(FbxNode* node, int32_t idx = 0, int32_t parentIdx = -1); // Made LoadBones overload explicit
    void LoadAnimationInfo();
    void LoadTransform(FbxNode* node, FbxMeshInfo* meshInfo);
    FbxAMatrix GetGlobalTransform(FbxNode* node);

    void LoadAnimationData(FbxMesh* mesh, FbxMeshInfo* meshInfo);
    void LoadBoneWeight(FbxCluster* cluster, int32_t boneIdx, FbxMeshInfo* meshInfo);
    void LoadOffsetMatrix(FbxCluster* cluster, const FbxAMatrix& matNodeTransform, int32_t boneIdx);
    void LoadKeyframe(int32_t animIndex, FbxNode* node, FbxCluster* cluster, const FbxAMatrix& matNodeTransform, int32_t boneIdx);

    int32_t FindBoneIndex(const string& name); // Changed parameter to const reference
    FbxAMatrix GetTransform(FbxNode* node);

    void FillBoneWeight(FbxMesh* mesh, FbxMeshInfo* meshInfo);
    FbxAMatrix ConvertToDXMatrix(const FbxAMatrix& fbxMatrix);

private:
    FbxManager* _manager = nullptr;
    FbxScene* _scene = nullptr;
    FbxImporter* _importer = nullptr;
    wstring         _resourceDirectory;

    vector<FbxMeshInfo>                 _meshes;
    vector<shared_ptr<FbxBoneInfo>>     _bones;
    vector<shared_ptr<FbxAnimClipInfo>> _animClips;
    FbxArray<FbxString*>                _animNames;

    // 제어 점 인덱스와 UV 좌표에 따른 고유 정점을 추적하기 위한 맵 생성.
    std::unordered_map<int32_t, std::vector<std::pair<int32_t, FbxVector2>>> uniqueVerticesMap;
};