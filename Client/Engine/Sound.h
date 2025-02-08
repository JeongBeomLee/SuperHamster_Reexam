// Sound.h
#pragma once
#include "Object.h"

class Sound : public Object
{
public:
    Sound();
    virtual ~Sound();

    virtual void Load(const wstring& path) override;

    // FMOD 사운드 객체 접근자
    FMOD::Sound* GetSound() const { return _sound; }
    FMOD::Channel* GetChannel() const { return _channel; }

    // 볼륨 제어
    void SetVolume(float volume);
    float GetVolume() const { return _volume; }

    // 기본 속성 설정
    void SetLoop(bool loop);
    bool IsLoop() const { return _loop; }

    // Channel 설정 (SoundSystem에서 사용)
    void SetChannel(FMOD::Channel* channel) { _channel = channel; }

private:
    FMOD::Sound* _sound = nullptr;
    FMOD::Channel* _channel = nullptr;
    float _volume = 1.0f;
    bool _loop = false;
};