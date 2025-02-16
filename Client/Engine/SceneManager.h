#pragma once

class Scene;

enum
{
	MAX_LAYER = 32
};

class SceneManager
{
private:
	SceneManager();
	~SceneManager();

public:
	static SceneManager* GetInstance()
	{
		static SceneManager instance;
		return &instance;
	}

public:
	enum class SceneType
    {
        MAIN_MENU,
        GAME_PLAY,
        GAME_CLEAR
    };

	void Update();
	void Render();
	void LoadScene(SceneType sceneType);

	void SetLayerName(uint8 index, const wstring& name);
	const wstring& IndexToLayerName(uint8 index) { return _layerNames[index]; }
	uint8 LayerNameToIndex(const wstring& name);

	shared_ptr<class GameObject> Pick(int32 screenX, int32 screenY);

public:
	shared_ptr<Scene> GetActiveScene() { return _activeScene; }

private:
	shared_ptr<Scene> LoadMainScene();
	shared_ptr<Scene> LoadGameScene();
	shared_ptr<Scene> LoadClearScene();
	void CreateFadeOutObjects(
		const Vec3& position,
		float width, float height, float y,
		float vOffset, float hOffset, float yOffset,
		shared_ptr<Scene>& scene);

private:
	shared_ptr<Scene> _activeScene;

	array<wstring, MAX_LAYER> _layerNames;
	map<wstring, uint8> _layerIndex;

	int32 _sceneChangeEventId;
};
