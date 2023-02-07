#ifndef __DB_CONNECTOR__H_
#define __DB_CONNECTOR__H_
#define STATIC_CONCPP
#include "MySQL/include/mysql/jdbc.h"
#include <Windows.h>
#include <thread>

#define MAX_QUERYLEN		512
#define SLOWQUERY_TIME		100
#define PROFILE_TERM		1000 * 60 * 10

namespace Jay
{
	class DBConnector
	{
		/**
		* @file		DBConnector.h
		* @brief	DB Connector Class
		* @details	DB ����� ���� Ŀ���� Ŭ����
		* @author   ������
		* @date		2023-02-07
		* @version  1.0.0
		**/
	private:
		struct PROFILE
		{
			DWORD64 iTotalTime;         // ��ü ���ð� ī���� Time. (��½� ȣ��ȸ���� ������ ��� ����)
			DWORD64 iMin;	            // �ּ� ���ð� ī���� Time. (�ʴ����� ����Ͽ� ����)
			DWORD64 iMax;	            // �ִ� ���ð� ī���� Time. (�ʴ����� ����Ͽ� ����)
			DWORD64 iCall;              // ���� ȣ�� Ƚ��.
			LARGE_INTEGER lStartTime;   // �������� ���� �ð�.
		};
	public:
		DBConnector();
		~DBConnector();
	public:
		/**
		* @brief	DB ����
		* @details
		* @param	const wchar_t*(�ּ�), int(��Ʈ), const wchar_t*(����), const wchar_t*(�н�����), const wchar_t*(��Ű��), bool(�翬�� ����)
		* @return	bool(���� ����)
		**/
		bool Connect(const wchar_t* ipaddress, int port, const wchar_t* user, const wchar_t* passwd, const wchar_t* db, bool reconnect = true);

		/**
		* @brief	DB ���� ����
		* @details
		* @param	void
		* @return	void
		**/
		void Disconnect(void);

		/**
		* @brief	DDL, DCL, TCL ���� ����
		* @details	������ ���Ǿ�, ������ �����, Ʈ������ ����� ���� ����
		* @param	const wchar_t*(����)
		* @return	void
		**/
		void Execute(const wchar_t* query, ...);

		/**
		* @brief	DML �� �����Ϳ� ������ ���ϴ� ���� ����
		* @details	INSERT, UPDATE, DELETE ���� ����
		* @param	const wchar_t*(����)
		* @return	void
		**/
		void ExecuteUpdate(const wchar_t* query, ...);

		/**
		* @brief	DML �� �����͸� ��ȸ�ϴ� ���� ����
		* @details	SELECT ���� ����
		* @param	const wchar_t*(����)
		* @return	sql::ResultSet*(��ȸ�� ������)
		**/
		sql::ResultSet* ExecuteQuery(const wchar_t* query, ...);

		/**
		* @brief	��ȸ�� ������ �ݳ�
		* @details	SELECT ������ ���� ��ȸ�� ������ �ݳ�
		* @param	sql::ResultSet*(��ȸ�� ������)
		* @return	void
		**/
		void ClearQuery(sql::ResultSet* res);

		/**
		* @brief	���� �ڵ� ���
		* @details
		* @param	void
		* @return	int(���� �ڵ�)
		**/
		int GetErrorCode(void);

		/**
		* @brief	���� �޽��� ���
		* @details
		* @param	void
		* @return	const wchar_t*(���� �޽���)
		**/
		const wchar_t* GetErrorMessage(void);

		/**
		* @brief	SQL ���� ���
		* @details
		* @param	void
		* @return	const wchar_t*(SQL ����)
		**/
		const wchar_t* GetSQLState(void);
	private:
		void ProfileBegin(void);
		void ProfileEnd(void);
		void ProfileDataOutText(void);
		void ProfileThread(void);
	private:
		sql::Driver* _driver;
		sql::Connection* _conn;
		sql::Statement* _stmt;
		wchar_t _queryw[MAX_QUERYLEN];
		char _query[MAX_QUERYLEN];
		int _errCode;
		wchar_t _errMessage[256];
		wchar_t _sqlState[256];
		PROFILE _profile;
		LARGE_INTEGER _freqency;
		HANDLE _hExitThreadEvent;
		std::thread* _profileThread;
	};
}

#endif