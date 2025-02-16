#pragma once

enum class REMOTE_PLAYER_STATE {
    IDLE,
    RUN,
    ROLL,
    AIM,
    FIRE,
    HIT,
    GETUP,
    END
};

class RemotePlayerState;
struct NetworkInputData;
class RemotePlayerStateMachine {
public:
    void Update(float deltaTime);
    void ChangeState(REMOTE_PLAYER_STATE newState);
    REMOTE_PLAYER_STATE GetCurrentState() const { return _currentState; }

    template<typename T>
    void RegisterState(REMOTE_PLAYER_STATE state) {
        _states[state] = std::make_unique<T>();
    }

    void SetOwner(class Player* owner) { _owner = owner; }
    void ProcessNetworkInput(const NetworkInputData& inputData);

private:
    REMOTE_PLAYER_STATE _currentState = REMOTE_PLAYER_STATE::IDLE;
    std::unordered_map<REMOTE_PLAYER_STATE, std::unique_ptr<RemotePlayerState>> _states;
    Player* _owner = nullptr;
};