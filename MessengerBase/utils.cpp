#include "utils.h"
#include <codecvt>
#include <ctime>

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


static const std::string base64_chars =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";

static bool is_base64(unsigned char ch) 
{
	return isalnum(ch) || (ch == '+') || (ch == '/');
}

std::string Base64_encode(const std::vector<unsigned char>& data) 
{
	std::string result;
	int i = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	for (auto& ch : data)
	{
		char_array_3[i++] = ch;
		if (i == 3) 
		{
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; i < 4; i++)
				result += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for (unsigned j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (int j = 0; j < i + 1; j++)
			result += base64_chars[char_array_4[j]];

		while (i++ < 3)
			result += '=';
	}

	return result;

}

std::vector<unsigned char> Base64_decode(const std::string& data)
{
	int in_len = data.size();
	int i = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::vector<unsigned char> ret;

	while (in_len-- && data[in_] != '=' && is_base64(data[in_])) 
	{
		char_array_4[i++] = data[in_]; in_++;
		if (i == 4) 
		{
			for (i = 0; i <4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; i < 3; i++)
				ret.push_back(char_array_3[i]);
			i = 0;
		}
	}

	if (i) 
	{
		for (unsigned j = i; j <4; j++)
			char_array_4[j] = 0;

		for (unsigned j = 0; j <4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (int j = 0; j < i - 1; j++)
			ret.push_back(char_array_3[j]);
	}

	return ret;
}

std::string TimeToString(time_t time)
{
	std::tm ptm;
	localtime_s(&ptm, &time);
	char buffer[32];
	std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", &ptm);
	return buffer;
}