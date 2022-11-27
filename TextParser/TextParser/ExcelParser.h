#ifndef __EXCEL_PARSER__H_
#define __EXCEL_PARSER__H_
#include <vector>
#include <string>

namespace Jay
{
	/**
	* @file		ExcelParser.h
	* @brief	���� ���� �ļ� Ŭ����
	* @details	CSV, TSV ���� ������ �����ϴ� ���� ���� �ļ�
	* @author   ������
	* @date		2022-09-01
	* @version  1.0.0
	**/
	class ExcelParser
	{
	public:
		enum FORMAT
		{
			CSV = 0,
			TSV
		};
	public:
		ExcelParser(FORMAT format = CSV);
		~ExcelParser();
	public:
		bool LoadFile(const wchar_t* filepath);
		bool SaveFile();
		void SelectRow(int y);
		void NextRow();
		void PrevRow();
		void MoveColumn(int x);
		void NextColumn();
		void PrevColumn();
		void GetColumn(int* value);
		void GetColumn(wchar_t* value);
		void PutColumn(int value);
		void PutColumn(const wchar_t* value);
	private:
		std::vector<std::vector<std::wstring>> _contents;
		wchar_t _filepath[_MAX_PATH];
		wchar_t _delimiter;
		int _row;
		int _column;
	};
}

#endif
