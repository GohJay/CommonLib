#ifndef __UTIL__H_
#define __UTIL__H_

class Util
{
public:
	static bool ExistFile(const wchar_t* filepath);
	static void Rename(const wchar_t* oldfile, const wchar_t* newfile, bool force = false);
};

#endif
