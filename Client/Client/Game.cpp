#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Input.h"

Game::Game()
{
}

Game::~Game()
{
}

void Game::Init(const WindowInfo& info)
{
	GEngine->Init(info);
	GET_SINGLE(SceneManager)->LoadScene(SceneManager::SceneType::MAIN_MENU);
}

void Game::Update()
{
	GEngine->Update();
	if (INPUT->GetButtonDown(KEY_TYPE::F11)) {
		GEngine->ToggleFullscreen();
	}
}
