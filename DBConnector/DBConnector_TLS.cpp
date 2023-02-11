#include "stdafx.h"
#include "DBConnector_TLS.h"
#include "StringUtil.h"
#include <strsafe.h>

using namespace Jay;

DBConnector_TLS::DBConnector_TLS()
{
	_tlsDB = TlsAlloc();
}
DBConnector_TLS::~DBConnector_TLS()
{
	DBConnector* db;
	while (!_gcStack.Pop(db))
	{
		db->Disconnect();
		delete db;
	}

	TlsFree(_tlsDB);
}
void DBConnector_TLS::Connect(const wchar_t* ipaddress, int port, const wchar_t* user, const wchar_t* passwd, const wchar_t* database, bool reconnect)
{
	DBConnector* db = GetDB();
	db->Connect(ipaddress, port, user, passwd, database, reconnect);
}
void DBConnector_TLS::Disconnect()
{
	DBConnector* db = GetDB();
	db->Disconnect();
}
void DBConnector_TLS::Execute(const wchar_t* query, ...)
{
	WCHAR queryw[MAX_QUERYLEN];
	va_list args;
	va_start(args, query);
	StringCchVPrintf(queryw, MAX_QUERYLEN, query, args);
	va_end(args);

	DBConnector* db = GetDB();
	db->Execute(queryw);
}
void DBConnector_TLS::ExecuteUpdate(const wchar_t* query, ...)
{
	WCHAR queryw[MAX_QUERYLEN];
	va_list args;
	va_start(args, query);
	StringCchVPrintf(queryw, MAX_QUERYLEN, query, args);
	va_end(args);

	DBConnector* db = GetDB();
	db->ExecuteUpdate(queryw);
}
sql::ResultSet* DBConnector_TLS::ExecuteQuery(const wchar_t* query, ...)
{
	WCHAR queryw[MAX_QUERYLEN];
	va_list args;
	va_start(args, query);
	StringCchVPrintf(queryw, MAX_QUERYLEN, query, args);
	va_end(args);

	DBConnector* db = GetDB();
	return db->ExecuteQuery(queryw);
}
void DBConnector_TLS::ClearQuery(sql::ResultSet* res)
{
	DBConnector* db = GetDB();
	db->ClearQuery(res);
}
DBConnector* DBConnector_TLS::GetDB()
{
	DBConnector* db = (DBConnector*)TlsGetValue(_tlsDB);
	if (db == NULL)
	{
		db = new DBConnector();
		_gcStack.Push(db);
		TlsSetValue(_tlsDB, db);
	}
	return db;
}
