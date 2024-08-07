#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Input.h"

void Game::Init(const WindowInfo& info)
{
	GEngine->Init(info);

    _networkManager = std::make_unique<NetworkManager>();
    if (_networkManager->Connect(SERVER_IP, PORT))
    {
        // ��Ŷ �ڵ鷯 ���
        RegisterPacketHandlers();
        // �α��� ��Ŷ ����
        C2S_LoginPacket* loginPacket = new C2S_LoginPacket();
        loginPacket->type = PacketType::C2S_LOGIN;
        loginPacket->size = sizeof(C2S_LoginPacket);
        _networkManager->SendPacket(loginPacket);
    }
    else
    {
        // ���� ����
        std::cerr << "Failed to connect to server" << std::endl;
    }

	GET_SINGLE(SceneManager)->LoadScene(L"TestScene");
}

void Game::Update()
{
	GEngine->Update();
	if (INPUT->GetButtonDown(KEY_TYPE::F11))
	{
		GEngine->ToggleFullscreen();
	}

    NetworkUpdate();
}

void Game::NetworkUpdate()
{
    _networkManager->Update();
}

void Game::RegisterPacketHandlers()
{
    _networkManager->RegisterHandler(PacketType::S2C_LOGIN_RESULT,
        [this](PacketHeader* packet) { Handle_S2C_LOGIN_RESULT(packet); });
    _networkManager->RegisterHandler(PacketType::S2C_GAME_START_RESULT,
        [this](PacketHeader* packet) { Handle_S2C_GAME_START_RESULT(packet); });
    _networkManager->RegisterHandler(PacketType::S2C_MOVE_RESULT,
        [this](PacketHeader* packet) { Handle_S2C_MOVE_RESULT(packet); });
    _networkManager->RegisterHandler(PacketType::S2C_ATTACK_RESULT,
        [this](PacketHeader* packet) { Handle_S2C_ATTACK_RESULT(packet); });
    _networkManager->RegisterHandler(PacketType::S2C_INTERACTION_RESULT,
        [this](PacketHeader* packet) { Handle_S2C_INTERACTION_RESULT(packet); });
}

void Game::Handle_S2C_LOGIN_RESULT(PacketHeader* packet)
{
    S2C_LoginResultPacket* loginResult = reinterpret_cast<S2C_LoginResultPacket*>(packet);
    if (loginResult->success)
    {
        std::cout << "Login successful. My Player ID: " << loginResult->playerId << std::endl;
        // ���� ���� ��û ���� �߰� ����
    }
    else
    {
        std::cout << "Login failed." << std::endl;
    }
}

void Game::Handle_S2C_GAME_START_RESULT(PacketHeader* packet)
{
    S2C_GameStartResultPacket* gameStartResult = reinterpret_cast<S2C_GameStartResultPacket*>(packet);
    if (gameStartResult->success)
    {
		std::cout << "Game started. " << std::endl;
        // ���� ���� ���� �߰� ����
    }
    else
    {
        std::cout << "Failed to start game." << std::endl;
    }
}

void Game::Handle_S2C_MOVE_RESULT(PacketHeader* packet)
{
    S2C_MoveResultPacket* moveResult = reinterpret_cast<S2C_MoveResultPacket*>(packet);
    // �÷��̾� �Ǵ� ���� ������Ʈ�� ��ġ ������Ʈ
    std::cout << "Player " << moveResult->playerId << " moved to ("
        << moveResult->posX << ", " << moveResult->posY << ", " << moveResult->posZ << ")" << std::endl;
}

void Game::Handle_S2C_ATTACK_RESULT(PacketHeader* packet)
{
    S2C_AttackResultPacket* attackResult = reinterpret_cast<S2C_AttackResultPacket*>(packet);
    if (attackResult->hit)
    {
        std::cout << "Player " << attackResult->playerId << " hit target " << attackResult->targetId
            << " for " << attackResult->damage << " damage." << std::endl;
        // ������ ����, ����Ʈ ǥ�� ���� �߰� ����
    }
    else
    {
        std::cout << "Player " << attackResult->playerId << " missed." << std::endl;
    }
}

void Game::Handle_S2C_INTERACTION_RESULT(PacketHeader* packet)
{
    S2C_InteractionResultPacket* interactionResult = reinterpret_cast<S2C_InteractionResultPacket*>(packet);
    if (interactionResult->success)
    {
        std::cout << "Player " << interactionResult->playerId << " interacted with "
            << interactionResult->targetId << std::endl;
        // ��ȣ�ۿ� ��� ó�� ����
    }
    else
    {
        std::cout << "Interaction failed." << std::endl;
    }
}