#include "utils.h"
#include <codecvt>

////////////////////////////////////////////

#ifdef _WIN32

#include <Windows.h>
void sleep(int msec)
{
	Sleep(msec);
}

#else

void slepp(int msec)
{
}

#endif

////////////////////////////////////////////

std::wstring ConvertUTF8_UTF16(std::string str)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(str.c_str());
}

std::string ConvertUTF16_UTF8(std::wstring str)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(str);
}
