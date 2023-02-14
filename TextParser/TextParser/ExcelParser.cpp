#include "ExcelParser.h"
#include "FileUtil.h"
#include <fstream>
#include <string.h>

using namespace Jay;
ExcelParser::ExcelParser(FORMAT format) : _row(0), _column(0)
{
	switch (format)
	{
	case FORMAT::CSV:
		_delimiter = L',';
		break;
	case FORMAT::TSV:
		_delimiter = L'\t';
		break;
	default:
		break;
	}
}
ExcelParser::~ExcelParser()
{
}
bool ExcelParser::LoadFile(const wchar_t * filepath)
{
	std::wifstream file(filepath);
	if (!file.is_open())
		return false;

	std::wstring line;
	int index = 0;
	while (getline(file, line))
	{
		size_t pos;
		size_t offset = 0;
		_contents.push_back(std::vector<std::wstring>());
		do
		{
			pos = line.find(_delimiter, offset);
			std::wstring token = line.substr(offset, pos - offset);
			_contents[index].push_back(std::move(token));
			offset = pos + 1;
		} while (pos != std::wstring::npos);
		index++;
	}
	file.close();

	wcscpy_s(_filepath, sizeof(_filepath) / 2, filepath);
	return true;
}
bool ExcelParser::SaveFile()
{
	if (!ExistFile(_filepath))
		return false;

	wchar_t lockFile[_MAX_PATH];
	wcscpy_s(lockFile, sizeof(lockFile) / 2, _filepath);
	wcscat_s(lockFile, L".lock");

	std::wofstream file(lockFile);
	if (!file.is_open())
		return false;

	for (int i = 0; i < _contents.size(); i++)
	{
		if (_contents[i].empty())
			continue;
		
		file << _contents[i][0];
		for (int j = 1; j < _contents[i].size(); j++)
		{
			file << _delimiter << _contents[i][j];
		}
		file << L'\n';
	}
	file.close();
	
	Rename(lockFile, _filepath, true);
	return true;
}
void ExcelParser::SelectRow(int y)
{
	_row = y;
}
void ExcelParser::NextRow()
{
	_row++;
}
void ExcelParser::PrevRow()
{
	_row--;
}
void ExcelParser::MoveColumn(int x)
{
	_column = x;
}
void ExcelParser::NextColumn()
{
	_column++;
}
void ExcelParser::PrevColumn()
{
	_column--;
}
void ExcelParser::GetColumn(int * value)
{
	*value = std::stoi(_contents[_row][_column]);
}
void ExcelParser::GetColumn(wchar_t * value)
{
	wcscpy_s(value, _contents[_row][_column].length() + 1, _contents[_row][_column].c_str());
}
void ExcelParser::PutColumn(int value)
{
	_contents[_row][_column] = std::to_wstring(value);
}
void ExcelParser::PutColumn(const wchar_t * value)
{
	_contents[_row][_column] = value;
}
