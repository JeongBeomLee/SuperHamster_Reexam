#pragma once
#include "RemotePlayerStateMachine.h"

enum class NetworkRole {
    None,
    Host,
    Client
};

// ��Ʈ �÷��׷� ���ǵ� �Է� ����
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

// ��Ʈ��ũ�� ������ �Է� ������ ����ü
#pragma pack(push, 1) // ����ü�� 1����Ʈ ���ķ� ����
struct NetworkInputData {
    uint32_t playerID;     // �÷��̾� �ĺ���
    uint16_t inputFlags;   // �Է� ���� ��Ʈ �÷���
    REMOTE_PLAYER_STATE currentState;  // ���� ���� �߰�
    Vec3 position;                     // ��ġ ���� �߰�
    double timestamp;      // Ÿ�ӽ�����
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

    // ���� �Է� ���¸� ����
    uint16_t m_lastInputState{ InputFlags::NONE };
    std::array<uint8_t, 1024> m_receiveBuffer;
};