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

        // 비동기 소켓으로 설정
        m_socket.non_blocking(true);
        Logger::Instance().Info("NetworkManager 초기화됨, 포트: {}", port);
    }
    catch (const boost::system::system_error& e) {
        Logger::Instance().Error("NetworkManager 초기화 실패: {}", e.what());
    }
}

void NetworkManager::StartHost()
{
    if (m_role != NetworkRole::None) return;

    m_role = NetworkRole::Host;
    StartNetwork();
	Logger::Instance().Info("NetworkManager 호스트로 시작");
}

void NetworkManager::JoinHost(const std::string& hostIP)
{
    if (m_role != NetworkRole::None) return;

    try {
        m_role = NetworkRole::Client;
        m_remoteEndpoint = boost::asio::ip::udp::endpoint(
            boost::asio::ip::make_address(hostIP), // 수정된 부분
            SERVER_PORT
        );

        StartNetwork();
        Logger::Instance().Info("호스트에게 연결됨. IP: {}", hostIP);
    }
    catch (const boost::system::system_error& e) {
        Logger::Instance().Error("호스트에게 연결 실패: {}", e.what());
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

                // 60fps로 제한
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

    // 현재 입력 상태를 비트 플래그로 변환
    if (INPUT->GetButton(KEY_TYPE::UP))    currentState |= InputFlags::UP;
    if (INPUT->GetButton(KEY_TYPE::DOWN))  currentState |= InputFlags::DOWN;
    if (INPUT->GetButton(KEY_TYPE::LEFT))  currentState |= InputFlags::LEFT;
    if (INPUT->GetButton(KEY_TYPE::RIGHT)) currentState |= InputFlags::RIGHT;
    if (INPUT->GetButton(KEY_TYPE::SPACE)) currentState |= InputFlags::SPACE;
    if (INPUT->GetButton(KEY_TYPE::A))     currentState |= InputFlags::A;
    if (INPUT->GetButton(KEY_TYPE::S))     currentState |= InputFlags::S;

    // 입력 상태가 변경되었을 때만 전송
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
            // 클라이언트 endpoint가 유효한지 확인 후 전송
            if (m_clientEndpoint.port() != 0) { // 포트가 0이면 아직 초기화되지 않은 것으로 판단
                m_socket.send_to(
                    boost::asio::buffer(&data, sizeof(NetworkInputData)),
                    m_clientEndpoint,
                    0,
                    error
                );
                if (error) {
                    Logger::Instance().Error("input data 전송 실패: {}", error.message());
                }
            }
        }
        else { // 클라이언트 모드
            m_socket.send_to(
                boost::asio::buffer(&data, sizeof(NetworkInputData)),
                m_remoteEndpoint,
                0,
                error
            );
            if (error) {
                Logger::Instance().Error("input data 전송 실패: {}", error.message());
            }
        }
    }
    catch (const std::exception& e) {
        Logger::Instance().Error("input data를 보내던 중 예외 발생: {}", e.what());
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

    // 호스트 모드일 경우, 최초로 받은 클라이언트 endpoint를 저장합니다.
    if (m_role == NetworkRole::Host && m_clientEndpoint.port() == 0) {
        m_clientEndpoint = senderEndpoint;
        Logger::Instance().Info("클라이언트 endpoint 설정됨: {}:{}",
            m_clientEndpoint.address().to_string(),
            m_clientEndpoint.port());
    }

    if (bytes == sizeof(NetworkInputData)) {
        NetworkInputData receivedData;
        std::memcpy(&receivedData, m_receiveBuffer.data(), bytes);

        // 네트워크 이벤트 발생
        GET_SINGLE(EventManager)->Publish(
            Event::NetworkInputEvent(receivedData)
        );
    }

    // 다음 수신 대기
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