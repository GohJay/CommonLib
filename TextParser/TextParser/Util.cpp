#include "Util.h"
#include <stdio.h>
#include <wchar.h>

bool Util::ExistFile(const wchar_t * filepath)
{
	FILE* pFile;
	if (_wfopen_s(&pFile, filepath, L"rb") != 0)
		return false;
	fclose(pFile);
	return true;
}
void Util::Rename(const wchar_t * oldfile, const wchar_t * newfile, bool force)
{
	if (force && ExistFile(newfile))
		_wremove(newfile);
	_wrename(oldfile, newfile);
}
