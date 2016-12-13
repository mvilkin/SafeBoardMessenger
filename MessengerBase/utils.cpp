#include "utils.h"
#include <codecvt>

////////////////////////////////////////////

#ifdef _WIN32

#include <Windows.h>
#include <direct.h>

void sleep(int msec)
{
	Sleep(msec);
}

std::string GetCurrentDir()
{
	const size_t current_dir_size = 512;
	char current_dir[current_dir_size];
	_getcwd(current_dir, current_dir_size);
	return current_dir;
}

bool CreateDir(std::string path)
{
	return CreateDirectoryA(path.c_str(), NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError();
}

bool CreateDir(std::wstring path)
{
	return CreateDirectoryW(path.c_str(), NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError();
}

#else

void slepp(int msec)
{
}

std::string GetCurrentDir()
{
	return std::string();
}

bool CreateDir(std::string path)
{
	return false;
}

bool CreateDir(std::wstring path)
{
	return false;
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
