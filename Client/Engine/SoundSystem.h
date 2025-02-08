#pragma once

class Sound;
class SoundSystem
{
    DECLARE_SINGLE(SoundSystem);

public:
    void Init();
    void Update();
    void CreateSound(const wstring& path, FMOD::Sound** sound);

    // 기본적인 사운드 제어
    void Play(shared_ptr<Sound> sound);
    void Stop(shared_ptr<Sound> sound);
    void StopAll();

    // 3D 사운드 관련
    void Play3D(shared_ptr<Sound> sound, const Vec3& pos);
    void Set3DListenerPosition(const Vec3& pos, const Vec3& forward, const Vec3& up);

    // FMOD 시스템 접근자 (Sound 클래스에서 사용)
    FMOD::System* GetSystem() { return _system; }

private:
    void CheckError(FMOD_RESULT result);

private:
    FMOD::System* _system = nullptr;
    FMOD_RESULT _lastResult;
};