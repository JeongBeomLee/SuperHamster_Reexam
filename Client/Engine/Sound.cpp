#include "pch.h"
#include "Sound.h"
#include "SoundSystem.h"

Sound::Sound() : Object(OBJECT_TYPE::SOUND)
{
}

Sound::~Sound()
{
    if (_sound)
    {
        _sound->release();
        _sound = nullptr;
    }
}

void Sound::Load(const wstring& path)
{
    // 기존 사운드가 있다면 해제
    if (_sound)
    {
        _sound->release();
        _sound = nullptr;
    }

	GET_SINGLE(SoundSystem)->CreateSound(path, &_sound);
}

void Sound::SetVolume(float volume)
{
    _volume = std::clamp(volume, 0.0f, 100.0f);
    if (_channel)
    {
        _channel->setVolume(_volume);
    }
}

void Sound::SetLoop(bool loop)
{
    _loop = loop;
    if (_sound)
    {
        //_sound->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
    }
}