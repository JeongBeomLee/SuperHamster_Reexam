#include "pch.h"
#include "EnginePch.h"
#include "Engine.h"

unique_ptr<Engine> GEngine = make_unique<Engine>();

wstring s2ws(const string& s)
{
	int32 len;
	int32 slength = static_cast<int32>(s.length()) + 1;
	len = ::MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	::MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	wstring ret(buf);
	delete[] buf;
	return ret;
}

string ws2s(const wstring& s)
{
	int32 len;
	int32 slength = static_cast<int32>(s.length()) + 1;
	len = ::WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
	string r(len, '\0');
	::WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
	return r;
}

float repeat(float t, float length)
{
	return t - floor(t / length) * length;
}

float ScalarLerpAngle(float a, float b, float t)
{
	float delta = repeat((b - a), XM_2PI);
	if (delta > XM_PI)
		delta -= XM_2PI;
	return a + delta * t;
}

void QuaternionToEulerAngles(FXMVECTOR q, XMFLOAT3* pEuler)
{
	XMFLOAT4 quat;
	XMStoreFloat4(&quat, q);

	// 오일러 각 계산
	float ysqr = quat.y * quat.y;

	// roll (X축 회전)
	float t0 = +2.0f * (quat.w * quat.x + quat.y * quat.z);
	float t1 = +1.0f - 2.0f * (quat.x * quat.x + ysqr);
	pEuler->x = std::atan2(t0, t1);

	// pitch (Y축 회전)
	float t2 = +2.0f * (quat.w * quat.y - quat.z * quat.x);
	t2 = t2 > +1.0f ? +1.0f : t2;
	t2 = t2 < -1.0f ? -1.0f : t2;
	pEuler->y = std::asin(t2);

	// yaww (Z축 회전)
	float t3 = +2.0f * (quat.w * quat.z + quat.x * quat.y);
	float t4 = +1.0f - 2.0f * (ysqr + quat.z * quat.z);
	pEuler->z = std::atan2(t3, t4);
}