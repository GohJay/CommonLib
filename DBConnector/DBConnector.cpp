#include "DBConnector.h"
#include "Logger.h"
#include "CrashDump.h"
#include "StringUtil.h"
#include <strsafe.h>
#include <memory>
#pragma comment(lib, "MySQL/lib64/vs14/mysqlcppconn-static.lib")

using namespace Jay;

DBConnector::DBConnector()
{
    _driver = get_driver_instance();

    memset(&_profile, 0, sizeof(_profile));
    QueryPerformanceFrequency(&_freqency);

    _hExitThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    _profileThread = new std::thread(&DBConnector::ProfileThread, this);
}
DBConnector::~DBConnector()
{
    SetEvent(_hExitThreadEvent);
    _profileThread->join();

    delete _profileThread;
    CloseHandle(_hExitThreadEvent);
}
bool DBConnector::Connect(const wchar_t* ipaddress, int port, const wchar_t* user, const wchar_t* passwd, const wchar_t* db, bool reconnect)
{
    char hostName[16];
    char userName[32];
    char password[32];
    char database[32];

    W2M(ipaddress, hostName, sizeof(hostName));
    W2M(user, userName, sizeof(userName));
    W2M(passwd, password, sizeof(password));
    W2M(db, database, sizeof(database));

    sql::ConnectOptionsMap property;

    try
    {
        //--------------------------------------------------------------------
        // DB ���� ���� ����
        //--------------------------------------------------------------------
        property["hostName"] = hostName;
        property["port"] = port;
        property["userName"] = userName;
        property["password"] = password;
        property["schema"] = database;
        property["OPT_RECONNECT"] = reconnect;

        //--------------------------------------------------------------------
        // DB ����
        //--------------------------------------------------------------------
        _conn = _driver->connect(property);

        //--------------------------------------------------------------------
        // Statement �Ҵ�
        //--------------------------------------------------------------------
        _stmt = _conn->createStatement();
    }
    catch (sql::SQLException& ex)
    {
        _errCode = ex.getErrorCode();
        M2W(ex.what(), _errMessage, sizeof(_errMessage) / 2);
        M2W(ex.getSQLStateCStr(), _sqlState, sizeof(_sqlState) / 2);

        Logger::WriteLog(L"DBConnector_Error"
            , LOG_LEVEL_ERROR
            , L"%s() - ErrorCode: %d, ErrorMessage: %s, SQLState: %s"
            , __FUNCTIONW__
            , _errCode
            , _errMessage
            , _sqlState);

        return false;
    }

	return true;
}
void DBConnector::Disconnect()
{
    delete _stmt;
    delete _conn;
}
void DBConnector::Execute(const wchar_t* query, ...)
{
    va_list args;
    va_start(args, query);
    StringCchVPrintf(_queryw, MAX_QUERYLEN, query, args);
    va_end(args);
    W2M(_queryw, _query, MAX_QUERYLEN);

    try
    {
        //--------------------------------------------------------------------
        // Statement �ʱ�ȭ
        //--------------------------------------------------------------------
        _stmt->clearAttributes();

        //--------------------------------------------------------------------
        // ���� ����
        //--------------------------------------------------------------------
        ProfileBegin();
        _stmt->execute(_query);
        ProfileEnd();
    }
    catch (sql::SQLException& ex)
    {
        _errCode = ex.getErrorCode();
        M2W(ex.what(), _errMessage, sizeof(_errMessage) / 2);
        M2W(ex.getSQLStateCStr(), _sqlState, sizeof(_sqlState) / 2);

        Logger::WriteLog(L"DBConnector_Error"
            , LOG_LEVEL_ERROR
            , L"Query: %s, ErrorCode: %d, ErrorMessage: %s, SQLState: %s"
            , _queryw
            , _errCode
            , _errMessage
            , _sqlState);

        //--------------------------------------------------------------------
        // DB ���� ���д� Fatal Error �̹Ƿ� ũ���ÿ� �Բ� �޸� ������ �����.
        //--------------------------------------------------------------------
        CrashDump::Crash();
    }
}
void DBConnector::ExecuteUpdate(const wchar_t* query, ...)
{
    va_list args;
    va_start(args, query);
    StringCchVPrintf(_queryw, MAX_QUERYLEN, query, args);
    va_end(args);
    W2M(_queryw, _query, MAX_QUERYLEN);

    try
    {
        //--------------------------------------------------------------------
        // Statement �ʱ�ȭ
        //--------------------------------------------------------------------
        _stmt->clearAttributes();

        //--------------------------------------------------------------------
        // ���� ����
        //--------------------------------------------------------------------
        ProfileBegin();
        _stmt->executeUpdate(_query);
        ProfileEnd();
    }
    catch (sql::SQLException& ex)
    {
        _errCode = ex.getErrorCode();
        M2W(ex.what(), _errMessage, sizeof(_errMessage) / 2);
        M2W(ex.getSQLStateCStr(), _sqlState, sizeof(_sqlState) / 2);

        Logger::WriteLog(L"DBConnector_Error"
            , LOG_LEVEL_ERROR
            , L"Query: %s, ErrorCode: %d, ErrorMessage: %s, SQLState: %s"
            , _queryw
            , _errCode
            , _errMessage
            , _sqlState);

        //--------------------------------------------------------------------
        // DB ���� ���д� Fatal Error �̹Ƿ� ũ���ÿ� �Բ� �޸� ������ �����.
        //--------------------------------------------------------------------
        CrashDump::Crash();
    }
}
sql::ResultSet* DBConnector::ExecuteQuery(const wchar_t* query, ...)
{
    va_list args;
    va_start(args, query);
    StringCchVPrintf(_queryw, MAX_QUERYLEN, query, args);
    va_end(args);
    W2M(_queryw, _query, MAX_QUERYLEN);

    sql::ResultSet* res;

    try
    {
        //--------------------------------------------------------------------
        // Statement �ʱ�ȭ
        //--------------------------------------------------------------------
        _stmt->clearAttributes();

        //--------------------------------------------------------------------
        // ���� ����
        //--------------------------------------------------------------------
        ProfileBegin();
        res = _stmt->executeQuery(_query);
        ProfileEnd();
    }
    catch (sql::SQLException& ex)
    {
        _errCode = ex.getErrorCode();
        M2W(ex.what(), _errMessage, sizeof(_errMessage) / 2);
        M2W(ex.getSQLStateCStr(), _sqlState, sizeof(_sqlState) / 2);

        Logger::WriteLog(L"DBConnector_Error"
            , LOG_LEVEL_ERROR
            , L"Query: %s, ErrorCode: %d, ErrorMessage: %s, SQLState: %s"
            , _queryw
            , _errCode
            , _errMessage
            , _sqlState);

        //--------------------------------------------------------------------
        // DB ���� ���д� Fatal Error �̹Ƿ� ũ���ÿ� �Բ� �޸� ������ �����.
        //--------------------------------------------------------------------
        CrashDump::Crash();
        return nullptr;
    }

    return res;
}
void DBConnector::ClearQuery(sql::ResultSet* res)
{
    delete res;
}
int DBConnector::GetErrorCode()
{
    return _errCode;
}
const wchar_t* DBConnector::GetErrorMessage()
{
    return _errMessage;
}
const wchar_t* DBConnector::GetSQLState()
{
    return _sqlState;
}
void DBConnector::ProfileBegin()
{
    //--------------------------------------------------------------------
    // �������ϸ� ����
    //--------------------------------------------------------------------
    QueryPerformanceCounter(&_profile.lStartTime);
}
void DBConnector::ProfileEnd()
{
    //--------------------------------------------------------------------
    // �������ϸ� ����
    //--------------------------------------------------------------------
    LARGE_INTEGER lEndTime;
    QueryPerformanceCounter(&lEndTime);

    //--------------------------------------------------------------------
    // �������ϸ��� ����� �޸𸮿� ���
    //--------------------------------------------------------------------
    DWORD64 between = lEndTime.QuadPart - _profile.lStartTime.QuadPart;
    if (_profile.iMin > between || _profile.iMin == 0)
        _profile.iMin = between;

    if (_profile.iMax < between)
        _profile.iMax = between;

    _profile.iTotalTime += between;
    _profile.iCall++;

    //--------------------------------------------------------------------
    // SlowQuery ���� �Ǵ�
    //--------------------------------------------------------------------
    LONGLONG millisecond = _freqency.QuadPart / 1000;
    double queryTime = (double)between / millisecond;
    if (queryTime >= SLOWQUERY_TIME)
    {
        Logger::WriteLog(L"DBConnector_SlowQuery"
            , LOG_LEVEL_SYSTEM
            , L"SlowQuery - Query: %s, QueryTime: %.4lfms"
            , _queryw
            , queryTime);
    }
}
void DBConnector::ProfileDataOutText()
{
    //--------------------------------------------------------------------
    // �������ϸ��� �����͸� ���Ͽ� ���
    //--------------------------------------------------------------------
    if (_profile.iCall <= 0)
        return;

    LONGLONG millisecond = _freqency.QuadPart / 1000;
    double max = (double)_profile.iMax / millisecond;
    double min = (double)_profile.iMin / millisecond;
    unsigned long long call = _profile.iCall;
    double totaltime = _profile.iTotalTime;
    double average = (double)(totaltime / call) / millisecond;

    Logger::WriteLog(L"DBConnector_Profile"
        , LOG_LEVEL_SYSTEM
        , L"Profile - ThreadID: 0x%04X, Average: %.4lfms, Max: %.4lfms, Min: %.4lfms, Call: %I64d"
        , GetCurrentThreadId()
        , average
        , max
        , min
        , call);
}
void DBConnector::ProfileThread()
{
    DWORD ret;
    while (1)
    {
        ret = WaitForSingleObject(_hExitThreadEvent, PROFILE_TERM);
        switch (ret)
        {
        case WAIT_OBJECT_0:
            return;
        case WAIT_TIMEOUT:
            ProfileDataOutText();
            break;
        default:
            break;
        }
    }
}
