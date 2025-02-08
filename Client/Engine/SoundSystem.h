#pragma once

class Sound;
class SoundSystem
{
    DECLARE_SINGLE(SoundSystem);

public:
    void Init();
    void Update();
    void CreateSound(const wstring& path, FMOD::Sound** sound);

    // �⺻���� ���� ����
    void Play(shared_ptr<Sound> sound);
    void Stop(shared_ptr<Sound> sound);
    void StopAll();

    // 3D ���� ����
    void Play3D(shared_ptr<Sound> sound, const Vec3& pos);
    void Set3DListenerPosition(const Vec3& pos, const Vec3& forward, const Vec3& up);

    // FMOD �ý��� ������ (Sound Ŭ�������� ���)
    FMOD::System* GetSystem() { return _system; }

private:
    void CheckError(FMOD_RESULT result);

private:
    FMOD::System* _system = nullptr;
    FMOD_RESULT _lastResult;
};