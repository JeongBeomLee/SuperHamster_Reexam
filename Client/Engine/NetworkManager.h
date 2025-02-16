#pragma once
#include "RemotePlayerStateMachine.h"

enum class NetworkRole {
    None,
    Host,
    Client
};

// 비트 플래그로 정의된 입력 상태
enum InputFlags : uint16_t {
    NONE = 0,
    UP = 1 << 0,
    DOWN = 1 << 1,
    LEFT = 1 << 2,
    RIGHT = 1 << 3,
    SPACE = 1 << 4,
    A = 1 << 5,
    S = 1 << 6
};

// 네트워크로 전송할 입력 데이터 구조체
#pragma pack(push, 1) // 구조체를 1바이트 정렬로 설정
struct NetworkInputData {
    uint32_t playerID;     // 플레이어 식별자
    uint16_t inputFlags;   // 입력 상태 비트 플래그
    REMOTE_PLAYER_STATE currentState;  // 현재 상태 추가
    Vec3 position;                     // 위치 정보 추가
    double timestamp;      // 타임스탬프
};
#pragma pack(pop)

class NetworkManager {
private:
    NetworkManager() : m_socket(m_ioContext) {}
	~NetworkManager() { Shutdown(); }
public:
	static NetworkManager* GetInstance()
	{
		static NetworkManager instance;
		return &instance;
	}

public:
    void Init(uint16_t port);
    void StartHost();
    void JoinHost(const std::string& hostIP);
    void Shutdown();

    bool IsConnected() const { return m_connected; }
    NetworkRole GetRole() const { return m_role; }

public:
    REMOTE_PLAYER_STATE ConvertToRemoteState(enum class PLAYER_STATE localState);

private:
    void StartNetwork();
    //void ProcessEvents();
    //void ProcessHostEvents();
	//void ProcessClientEvents();
    void CheckInputStateChange();
    void SendInputData(const NetworkInputData& data);
    void StartReceive();
    void HandleReceive(
        const boost::system::error_code& error, 
        size_t bytes, 
        boost::asio::ip::udp::endpoint senderEndpoint);

private:
    boost::asio::io_context m_ioContext;
    boost::asio::ip::udp::socket m_socket;
    boost::asio::ip::udp::endpoint m_remoteEndpoint;
    boost::asio::ip::udp::endpoint m_clientEndpoint;

    std::thread m_networkThread;
    std::atomic<bool> m_running{ false };
    std::atomic<bool> m_connected{ false };
    NetworkRole m_role{ NetworkRole::None };

    // 이전 입력 상태를 저장
    uint16_t m_lastInputState{ InputFlags::NONE };
    std::array<uint8_t, 1024> m_receiveBuffer;
};