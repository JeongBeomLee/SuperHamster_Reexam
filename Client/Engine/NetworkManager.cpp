#include "pch.h"
#include "NetworkManager.h"
#include "Timer.h"
#include "Input.h"
#include "EventManager.h"
#include "Engine.h"

void NetworkManager::Init(uint16_t port)
{
    try {
        m_socket = boost::asio::ip::udp::socket(
            m_ioContext,
            boost::asio::ip::udp::endpoint(
                boost::asio::ip::udp::v4(),
                port
            )
        );

        // �񵿱� �������� ����
        m_socket.non_blocking(true);
        Logger::Instance().Info("NetworkManager �ʱ�ȭ��, ��Ʈ: {}", port);
    }
    catch (const boost::system::system_error& e) {
        Logger::Instance().Error("NetworkManager �ʱ�ȭ ����: {}", e.what());
    }
}

void NetworkManager::StartHost()
{
    if (m_role != NetworkRole::None) return;

    m_role = NetworkRole::Host;
    StartNetwork();
	Logger::Instance().Info("NetworkManager ȣ��Ʈ�� ����");
}

void NetworkManager::JoinHost(const std::string& hostIP)
{
    if (m_role != NetworkRole::None) return;

    try {
        m_role = NetworkRole::Client;
        m_remoteEndpoint = boost::asio::ip::udp::endpoint(
            boost::asio::ip::make_address(hostIP), // ������ �κ�
            SERVER_PORT
        );

        StartNetwork();
        Logger::Instance().Info("ȣ��Ʈ���� �����. IP: {}", hostIP);
    }
    catch (const boost::system::system_error& e) {
        Logger::Instance().Error("ȣ��Ʈ���� ���� ����: {}", e.what());
        m_role = NetworkRole::None;
    }
}

void NetworkManager::StartNetwork()
{
    m_running = true;
    m_connected = true;

    m_networkThread = std::thread([this]() {
        StartReceive();

        while (m_running) {
            try {
                m_ioContext.poll();
                CheckInputStateChange();

                // 60fps�� ����
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }
            catch (const std::exception& e) {
                Logger::Instance().Error("Network thread error: {}", e.what());
            }
        }
        });
}

void NetworkManager::CheckInputStateChange()
{
    uint16_t currentState = InputFlags::NONE;

    // ���� �Է� ���¸� ��Ʈ �÷��׷� ��ȯ
    if (INPUT->GetButton(KEY_TYPE::UP))    currentState |= InputFlags::UP;
    if (INPUT->GetButton(KEY_TYPE::DOWN))  currentState |= InputFlags::DOWN;
    if (INPUT->GetButton(KEY_TYPE::LEFT))  currentState |= InputFlags::LEFT;
    if (INPUT->GetButton(KEY_TYPE::RIGHT)) currentState |= InputFlags::RIGHT;
    if (INPUT->GetButton(KEY_TYPE::SPACE)) currentState |= InputFlags::SPACE;
    if (INPUT->GetButton(KEY_TYPE::A))     currentState |= InputFlags::A;
    if (INPUT->GetButton(KEY_TYPE::S))     currentState |= InputFlags::S;

    // �Է� ���°� ����Ǿ��� ���� ����
    /*if (currentState != m_lastInputState) {
        NetworkInputData data;
        data.playerID = GEngine->GetMyPlayerId();
        data.inputFlags = currentState;
        data.timestamp = DELTA_TIME;

        SendInputData(data);
        m_lastInputState = currentState;
    }*/

    NetworkInputData data;
    data.playerID = GEngine->GetMyPlayerId();
    data.inputFlags = currentState;
    data.timestamp = DELTA_TIME;

    SendInputData(data);
}

void NetworkManager::SendInputData(const NetworkInputData& data)
{
    if (!m_connected) {
        return;
    }

    try {
        boost::system::error_code error;
        if (m_role == NetworkRole::Host) {
            // Ŭ���̾�Ʈ endpoint�� ��ȿ���� Ȯ�� �� ����
            if (m_clientEndpoint.port() != 0) { // ��Ʈ�� 0�̸� ���� �ʱ�ȭ���� ���� ������ �Ǵ�
                m_socket.send_to(
                    boost::asio::buffer(&data, sizeof(NetworkInputData)),
                    m_clientEndpoint,
                    0,
                    error
                );
                if (error) {
                    Logger::Instance().Error("input data ���� ����: {}", error.message());
                }
            }
        }
        else { // Ŭ���̾�Ʈ ���
            m_socket.send_to(
                boost::asio::buffer(&data, sizeof(NetworkInputData)),
                m_remoteEndpoint,
                0,
                error
            );
            if (error) {
                Logger::Instance().Error("input data ���� ����: {}", error.message());
            }
        }
    }
    catch (const std::exception& e) {
        Logger::Instance().Error("input data�� ������ �� ���� �߻�: {}", e.what());
    }
}

void NetworkManager::StartReceive()
{
    if (!m_connected) {
        return;
    }

    m_socket.async_receive_from(
        boost::asio::buffer(m_receiveBuffer),
        m_remoteEndpoint,
        [this](const boost::system::error_code& error, size_t bytes) {
            HandleReceive(error, bytes , m_remoteEndpoint);
        }
    );
}

void NetworkManager::HandleReceive(
    const boost::system::error_code& error, 
    size_t bytes, 
    boost::asio::ip::udp::endpoint senderEndpoint)
{
    if (error) {
        Logger::Instance().Error("Receive error: {}", error.message());
        return;
    }

    // ȣ��Ʈ ����� ���, ���ʷ� ���� Ŭ���̾�Ʈ endpoint�� �����մϴ�.
    if (m_role == NetworkRole::Host && m_clientEndpoint.port() == 0) {
        m_clientEndpoint = senderEndpoint;
        Logger::Instance().Info("Ŭ���̾�Ʈ endpoint ������: {}:{}",
            m_clientEndpoint.address().to_string(),
            m_clientEndpoint.port());
    }

    if (bytes == sizeof(NetworkInputData)) {
        NetworkInputData receivedData;
        std::memcpy(&receivedData, m_receiveBuffer.data(), bytes);

        // ��Ʈ��ũ �̺�Ʈ �߻�
        GET_SINGLE(EventManager)->Publish(
            Event::NetworkInputEvent(receivedData)
        );
    }

    // ���� ���� ���
    StartReceive();
}

void NetworkManager::Shutdown()
{
    m_running = false;
    m_connected = false;

    if (m_networkThread.joinable()) {
        m_networkThread.join();
    }

    try {
        if (m_socket.is_open()) {
            m_socket.close();
        }
        m_ioContext.stop();
    }
    catch (const std::exception& e) {
        Logger::Instance().Error("Error during shutdown: {}", e.what());
    }

    m_role = NetworkRole::None;
    Logger::Instance().Info("NetworkManager shutdown completed");
}