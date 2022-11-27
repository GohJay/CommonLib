#include "ConfigParser.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define BOMCODE_UTF16LE		0xFEFF
#define BOMCODE_UTF16_SIZE	2

using namespace Jay;
ConfigParser::ConfigParser() : _buffer(nullptr)
{
}
ConfigParser::~ConfigParser()
{
	delete[] _buffer;
}
bool ConfigParser::LoadFile(const wchar_t * filepath)
{
	FILE* pFile;
	if (_wfopen_s(&pFile, filepath, L"rb, ccs = UTF-16LE") != 0)
		return false;
	
	wchar_t bomCode;
	fread_s(&bomCode, BOMCODE_UTF16_SIZE, BOMCODE_UTF16_SIZE, 1, pFile);
	int offset = (bomCode == BOMCODE_UTF16LE) ? BOMCODE_UTF16_SIZE : 0;
	int filesize;
	fseek(pFile, 0, SEEK_END);
	filesize = ftell(pFile) - offset;
	fseek(pFile, offset, SEEK_SET);

	_buffer = new wchar_t[filesize / 2];
	_bufferEnd = _buffer + (filesize / 2);
	fread_s(_buffer, filesize, filesize, 1, pFile);
	fclose(pFile);
	return true;
}
bool ConfigParser::GetValue(const wchar_t * section, const wchar_t * key, int * value)
{
	_bufferPos = _buffer;
	wchar_t *offset;
	wchar_t word[256];
	int len;
	while (GetNextWord(&offset, &len))
	{
		memmove_s(word, sizeof(word), offset, len * 2);
		word[len] = L'\0';
		if (wcscmp(word, section) == 0)		// SECTION
		{
			if (GetNextWord(&offset, &len))
			{
				memmove_s(word, sizeof(word), offset, len * 2);
				word[len] = L'\0';
				if (wcscmp(word, L"{") != 0)	// SECTION {
					continue;

				while (GetNextWord(&offset, &len))
				{
					memmove_s(word, sizeof(word), offset, len * 2);
					word[len] = L'\0';
					if (wcscmp(word, L"}") == 0)	// SECTION { }
						break;

					if (wcscmp(word, key) != 0)		// SECTION { KEY }
						continue;
					
					if (GetNextWord(&offset, &len))
					{
						memmove_s(word, sizeof(word), offset, len * 2);
						word[len] = L'\0';
						if (wcscmp(word, L"=") == 0)	// SECTION { KEY = }
						{
							if (GetNextWord(&offset, &len))
							{
								memmove_s(word, sizeof(word), offset, len * 2);
								if (word[0] != L'"' && word[len - 1] != L'"')	// SECTION { KEY = VALUE }
								{
									word[len] = '\0';
									*value = _wtoi(word);
									return true;
								}
							}
							return false;
						}
					}
					return false;
				}
			}
			return false;
		}
	}
	return false;
}
bool ConfigParser::GetValue(const wchar_t * section, const wchar_t * key, wchar_t * value)
{
	_bufferPos = _buffer;
	wchar_t *offset;
	wchar_t word[256];
	int len;
	while (GetNextWord(&offset, &len))
	{
		memmove_s(word, sizeof(word), offset, len * 2);
		word[len] = L'\0';		
		if (wcscmp(word, section) == 0)		// SECTION
		{
			if (GetNextWord(&offset, &len))
			{
				memmove_s(word, sizeof(word), offset, len * 2);
				word[len] = L'\0';				
				if (wcscmp(word, L"{") != 0)	// SECTION {
					continue;

				while (GetNextWord(&offset, &len))
				{
					memmove_s(word, sizeof(word), offset, len * 2);
					word[len] = L'\0';					
					if (wcscmp(word, L"}") == 0)	// SECTION { }
						break;

					if (wcscmp(word, key) != 0)		// SECTION { KEY }
						continue;

					if (GetNextWord(&offset, &len))
					{
						memmove_s(word, sizeof(word), offset, len * 2);
						word[len] = L'\0';
						if (wcscmp(word, L"=") == 0)	// SECTION { KEY = }
						{
							if (GetNextWord(&offset, &len))
							{
								memmove_s(word, sizeof(word), offset, len * 2);								
								if (word[0] == L'"' && word[len - 1] == L'"')	// SECTION { KEY = "VALUE" }
								{
									word[len - 1] = L'\0';
									wcscpy_s(value, sizeof(word) / 2, word + 1);
									return true;
								}
							}
							return false;
						}
					}
					return false;
				}
			}
			return false;
		}
	}
	return false;
}
bool ConfigParser::SkipNonCommand()
{
	while (_bufferPos < _bufferEnd)
	{
		// 주석 제거
		if (*_bufferPos == '/' && *(_bufferPos + 1) == '/')
		{
			_bufferPos += 2;
			while (_bufferPos < _bufferEnd)
			{
				if (*_bufferPos == '\n')
				{
					_bufferPos++;
					break;
				}
				_bufferPos++;
			}
			continue;
		}

		/* 주석 제거 */
		if (*_bufferPos == '/' && *(_bufferPos + 1) == '*')
		{
			_bufferPos += 2;
			while (_bufferPos < _bufferEnd)
			{
				if (*_bufferPos == '*' && *(_bufferPos + 1) == '/')
				{
					_bufferPos += 2;
					break;
				}
				_bufferPos++;
			}
			continue;
		}

		if (*_bufferPos == 0x20 /*스페이스*/ || *_bufferPos == 0x08 /*백스페이스*/ ||
			*_bufferPos == 0x0a /*라인피드*/ || *_bufferPos == 0x0d /*캐리지리턴*/ ||
			*_bufferPos == 0x09 /*탭*/ || *_bufferPos == ',')
		{
			_bufferPos++;
			continue;
		}

		return true;
	}
	return false;
}
bool ConfigParser::GetNextWord(wchar_t ** offset, int * len)
{
	if (SkipNonCommand())
	{
		*offset = _bufferPos;
		while (_bufferPos < _bufferEnd)
		{
			if (*_bufferPos == 0x20 /*스페이스*/ || *_bufferPos == 0x08 /*백스페이스*/ ||
				*_bufferPos == 0x0a /*라인피드*/ || *_bufferPos == 0x0d /*캐리지리턴*/ ||
				*_bufferPos == 0x09 /*탭*/ || *_bufferPos == ',')
			{
				*len = _bufferPos - *offset;
				return true;
			}
			_bufferPos++;
		}
	}
	return false;
}
