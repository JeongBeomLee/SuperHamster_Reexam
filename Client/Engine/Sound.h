// Sound.h
#pragma once
#include "Object.h"

class Sound : public Object
{
public:
    Sound();
    virtual ~Sound();

    virtual void Load(const wstring& path) override;

    // FMOD ���� ��ü ������
    FMOD::Sound* GetSound() const { return _sound; }
    FMOD::Channel* GetChannel() const { return _channel; }

    // ���� ����
    void SetVolume(float volume);
    float GetVolume() const { return _volume; }

    // �⺻ �Ӽ� ����
    void SetLoop(bool loop);
    bool IsLoop() const { return _loop; }

    // Channel ���� (SoundSystem���� ���)
    void SetChannel(FMOD::Channel* channel) { _channel = channel; }

private:
    FMOD::Sound* _sound = nullptr;
    FMOD::Channel* _channel = nullptr;
    float _volume = 1.0f;
    bool _loop = false;
};