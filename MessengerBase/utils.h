#ifndef _MESSENGERBASE_UTILS_H_
#define _MESSENGERBASE_UTILS_H_

#include <string>

void sleep(int msec);
std::string GetCurrentDir();
bool CreateDir(std::string path);
bool CreateDir(std::wstring path);
std::wstring ConvertUTF8_UTF16(std::string str);
std::string ConvertUTF16_UTF8(std::wstring str);

#endif // _MESSENGERBASE_UTILS_H_
