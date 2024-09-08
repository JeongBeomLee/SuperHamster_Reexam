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
	int myGameID = -1;
	cout << "My Game ID: " << endl;

	GEngine->SetMyPlayerId(myGameID);
	GEngine->Init(info);
	GET_SINGLE(SceneManager)->LoadScene(L"TestScene");
}

void Game::Update()
{
	GEngine->Update();
	if (INPUT->GetButtonDown(KEY_TYPE::F11))
	{
		GEngine->ToggleFullscreen();
	}
}
