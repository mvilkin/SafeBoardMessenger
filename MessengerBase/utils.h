#ifndef _MESSENGERBASE_UTILS_H_
#define _MESSENGERBASE_UTILS_H_

#include <string>
#include <vector>

void sleep(int msec);
std::string GetCurrentDir();
bool CreateDir(std::string path);
bool CreateDir(std::wstring path);
std::wstring ConvertUTF8_UTF16(std::string str);
std::string ConvertUTF16_UTF8(std::wstring str);
std::string Base64_encode(const std::vector<unsigned char>& data);
std::vector<unsigned char> Base64_decode(const std::string& data);
std::string TimeToString(time_t time);

#endif // _MESSENGERBASE_UTILS_H_
