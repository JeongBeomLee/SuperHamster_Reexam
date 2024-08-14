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

Vec4 Slerp(const Vec4& start, const Vec4& end, float ratio)
{
    Vec4 startCopy = start;
    Vec4 endCopy = end;

    float dot = startCopy.x * endCopy.x + startCopy.y * endCopy.y + startCopy.z * endCopy.z + startCopy.w * endCopy.w;
    if (dot < 0.0f)
    {
        endCopy = -endCopy;
        dot = -dot;
    }

    const float DOT_THRESHOLD = 0.9995f;
    if (dot > DOT_THRESHOLD)
    {
        Vec4 result = startCopy + ratio * (endCopy - startCopy);
        result.Normalize();
        return result;
    }

    float theta_0 = acos(dot);
    float theta = theta_0 * ratio;
    Vec4 end_orthogonal = endCopy - startCopy * dot;
    end_orthogonal.Normalize();

    return startCopy * cos(theta) + end_orthogonal * sin(theta);
}