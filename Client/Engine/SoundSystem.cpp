#include "pch.h"
#include "SoundSystem.h"
#include "Sound.h"
#include "Transform.h"

void SoundSystem::Init()
{
    _lastResult = FMOD::System_Create(&_system);
    CheckError(_lastResult);

    // 64개의 가상 채널 초기화
    _lastResult = _system->init(128, 
        FMOD_INIT_NORMAL | 
        FMOD_INIT_CHANNEL_DISTANCEFILTER |
        FMOD_INIT_VOL0_BECOMES_VIRTUAL, 
        nullptr);
    CheckError(_lastResult);

    // 거리 단위 설정 (1 unit = 1 meter)
    _system->set3DSettings(1.0f, 1.0f, 1.0f);
}

void SoundSystem::Update3DListenerPosition()
{
	if (_player == nullptr) return;
    auto transform = _player->GetGameObject()->GetTransform();
    // 사운드 시스템의 리스너 위치 설정 (TODO: 서버 추가하면 내 캐릭터의 위치로만 설정하도록 수정)
    Vec3 pos = transform->GetLocalPosition();

    // forward와 up 벡터를 정규화
    Vec3 forward = transform->GetForward();
    Vec3 up = transform->GetUp();

    forward.Normalize();
    up.Normalize();

    // forward와 up 벡터가 수직이 되도록 조정
    Vec3 right = forward.Cross(up);
    right.Normalize();
    up = right.Cross(forward);
    up.Normalize();

    Set3DListenerPosition(pos, forward, up);
}

void SoundSystem::Update()
{
    if (_system) {
        Update3DListenerPosition();
        _system->update();
    }
}

void SoundSystem::Play(shared_ptr<Sound> sound)
{
    if (sound == nullptr || sound->GetSound() == nullptr)
        return;

    FMOD::Channel* channel = nullptr;
    _lastResult = _system->playSound(sound->GetSound(), nullptr, false, &channel);

    if (_lastResult == FMOD_OK && channel)
    {
        // 채널 설정
        channel->setVolume(sound->GetVolume());
        channel->setMode(sound->IsLoop() ? FMOD_2D | FMOD_LOOP_NORMAL : FMOD_2D | FMOD_LOOP_OFF);

        // Sound 객체에 채널 설정
        sound->SetChannel(channel);
    }

    CheckError(_lastResult);
}

void SoundSystem::Play3D(shared_ptr<Sound> sound, const Vec3& pos)
{
    if (sound == nullptr || sound->GetSound() == nullptr)
        return;

    FMOD::Channel* channel = nullptr;
    _lastResult = _system->playSound(sound->GetSound(), nullptr, true, &channel);

    if (_lastResult == FMOD_OK && channel)
    {
        // 3D 속성 설정
        FMOD_VECTOR position = { pos.x, pos.y, pos.z };
        FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f };

        channel->set3DAttributes(&position, &velocity);
        channel->setVolume(sound->GetVolume());
        channel->setMode(sound->IsLoop() ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
        channel->setPaused(false);

        sound->SetChannel(channel);
    }

    CheckError(_lastResult);
}

void SoundSystem::Stop(shared_ptr<Sound> sound)
{
    if (sound && sound->GetChannel())
    {
        sound->GetChannel()->stop();
        sound->SetChannel(nullptr);
    }
}

void SoundSystem::StopAll()
{
    // 모든 채널 정지
    if (_system)
    {
        _lastResult = _system->playSound(nullptr, nullptr, true, nullptr);
        CheckError(_lastResult);
    }
}

void SoundSystem::Set3DListenerPosition(const Vec3& pos, const Vec3& forward, const Vec3& up)
{
    FMOD_VECTOR position = { pos.x, pos.y, pos.z };
    FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f };
    FMOD_VECTOR fwd = { forward.x, forward.y, forward.z };
    FMOD_VECTOR up_ = { up.x, up.y, up.z };

    _lastResult = _system->set3DListenerAttributes(0, &position, &velocity, &fwd, &up_);
    CheckError(_lastResult);
}

void SoundSystem::CreateSound(const wstring& path, FMOD::Sound** sound)
{
	string str = ws2s(path);
    _lastResult = _system->createSound(str.c_str(), FMOD_3D, nullptr, sound);
    CheckError(_lastResult);
}

void SoundSystem::CheckError(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        Logger::Instance().Error("FMOD Error: {}", FMOD_ErrorString(result));
    }
}