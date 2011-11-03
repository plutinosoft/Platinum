/*****************************************************************
|
|   Ozone - Sqlite3 C++ interface
|
|   Copyright (c) 2004-2008, Plutinosoft, LLC.
|   Author: Sylvain Rebaud (sylvain@plutinosoft.com)
|
****************************************************************/

#ifndef _OZN_DATABASE_H_
#define _OZN_DATABASE_H_

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "Neptune.h"
#include "sqlite3.h"
#include "OznResults.h"
#include "OznProperty.h"
#include "OznSchema.h"

/*----------------------------------------------------------------------
|   prototypes
+---------------------------------------------------------------------*/
NPT_Result OZN_MapErrorCode(int error);

/*----------------------------------------------------------------------
|   forward declarations
+---------------------------------------------------------------------*/
class OZN_Query;
class OZN_Statement;

/*----------------------------------------------------------------------
|   OZN_Database class
+---------------------------------------------------------------------*/
class OZN_Database
{
    friend class OZN_Statement;
    friend class OZN_Query;

public:
    ~OZN_Database();

    // class methods
    static NPT_Result Create(const char*    path, 
                             NPT_Int32      busy_timeout_ms, 
                             bool           create_file,
                             OZN_Database*& db);

    // accessor methods
    NPT_Int32  GetBusyTimeOut() { return m_BusyTimeoutMs; }

    // methods
    NPT_Result ExecuteDML(const char* sql, NPT_UInt32* changes = NULL);
    NPT_Result ExecuteScalar(const char* sql, OZN_Property& property);
    NPT_Result GetLastRowId(NPT_UInt32& id);
    NPT_Result Lock();
    NPT_Result Unlock();
    NPT_Result CheckTableSchema(const OZN_TableDescription& desc);

private:
    // methods
    OZN_Database(sqlite3* sq3_db, NPT_Int32 busy_timeout_ms);

private:
    sqlite3*  m_Sq3Db;
    NPT_Int32 m_BusyTimeoutMs;
};

#endif /* _OZN_DATABASE_H_ */