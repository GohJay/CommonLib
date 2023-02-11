#ifndef __DB_CONNECTOR_TLS__H_
#define __DB_CONNECTOR_TLS__H_
#include "DBConnector.h"
#include "LockFreeStack.h"

namespace Jay
{
	class DBConnector_TLS
	{
		/**
		* @file		DBConnector.h
		* @brief	DB Connector Class TLS 버전
		* @details	멀티스레드 환경에서의 DB 사용을 위한 커넥터 클래스
		* @author   고재현
		* @date		2023-02-11
		* @version  1.0.0
		**/
	public:
		DBConnector_TLS();
		~DBConnector_TLS();
	public:
		/**
		* @brief	DB 연결
		* @details
		* @param	const wchar_t*(주소), int(포트), const wchar_t*(유저), const wchar_t*(패스워드), const wchar_t*(데이터베이스), bool(재연결 여부)
		* @return	void
		**/
		void Connect(const wchar_t* ipaddress, int port, const wchar_t* user, const wchar_t* passwd, const wchar_t* database, bool reconnect = true);

		/**
		* @brief	DB 연결 종료
		* @details
		* @param	void
		* @return	void
		**/
		void Disconnect(void);

		/**
		* @brief	DDL, DCL, TCL 쿼리 실행
		* @details	데이터 정의어, 데이터 제어어, 트렌젝션 제어어 쿼리 실행
		* @param	const wchar_t*(쿼리)
		* @return	void
		**/
		void Execute(const wchar_t* query, ...);

		/**
		* @brief	DML 중 데이터에 변경을 가하는 쿼리 실행
		* @details	INSERT, UPDATE, DELETE 쿼리 실행
		* @param	const wchar_t*(쿼리)
		* @return	void
		**/
		void ExecuteUpdate(const wchar_t* query, ...);

		/**
		* @brief	DML 중 데이터를 조회하는 쿼리 실행
		* @details	SELECT 쿼리 실행
		* @param	const wchar_t*(쿼리)
		* @return	sql::ResultSet*(조회한 데이터)
		**/
		sql::ResultSet* ExecuteQuery(const wchar_t* query, ...);

		/**
		* @brief	조회한 데이터 반납
		* @details	SELECT 쿼리를 통해 조회한 데이터 반납
		* @param	sql::ResultSet*(조회한 데이터)
		* @return	void
		**/
		void ClearQuery(sql::ResultSet* res);
	private:
		DBConnector* GetDB();
	private:
		DWORD _tlsDB;
		LockFreeStack<DBConnector*> _gcStack;
	};
}

#endif
