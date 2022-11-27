#include <iostream>
#include "../TextParser/ExcelParser.h"
#include "../TextParser/ConfigParser.h"
#pragma comment(lib, "TextParser.lib")

using namespace std;
int main()
{
	//Jay::ConfigParser cnfParser;
	//cnfParser.LoadFile(L"Config.cnf");	
	//const wchar_t* section = L"SERVER";
	//const wchar_t* key = L"IP";
	//wchar_t value[64];
	//cnfParser.GetValue(section, key, value);
	//return 0;

	Jay::ExcelParser csvParser(Jay::ExcelParser::FORMAT::TSV);
	csvParser.LoadFile(L"Profiling.txt");
	for (int i = 0; i < 6; i++)
	{
		csvParser.MoveColumn(0);
		for (int j = 0; j < 5; j++)
		{
			wchar_t data[64];
			csvParser.GetColumn(data);
			csvParser.NextColumn();
			std::wcout << data << " ";
		}
		std::wcout << std::endl;
		csvParser.NextRow();
	}
	return 0;
}
