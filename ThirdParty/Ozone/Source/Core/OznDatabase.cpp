/*****************************************************************
|
|   Ozone - Sqlite3 C++ interface
|
|   Copyright (c) 2004-2008, Plutinosoft, LLC.
|   Author: Sylvain Rebaud (sylvain@plutinosoft.com)
|
****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "OznDatabase.h"
#include "OznSchema.h"
#include "OznSql.h"
#include "OznQuery.h"

/*----------------------------------------------------------------------
|   constants
+---------------------------------------------------------------------*/
#define BUSY_SLEEP_TIMEOUT_MS 100

/*----------------------------------------------------------------------
|   OZN_BusyHandler
+---------------------------------------------------------------------*/
static int 
OZN_BusyHandler(void* cookie, int numtries)
{
    NPT_TimeInterval busy_sleep_timeout(0, BUSY_SLEEP_TIMEOUT_MS * 1000000);

    if (cookie == NULL) 
        return 0;

    if (numtries >= ((OZN_Database*)cookie)->GetBusyTimeOut()/BUSY_SLEEP_TIMEOUT_MS) 
        return 0;

    NPT_System::Sleep(busy_sleep_timeout);
    return 1;
}

/*----------------------------------------------------------------------
|   OZN_MapErrorCode
+---------------------------------------------------------------------*/
NPT_Result 
OZN_MapErrorCode(int error)
{
    switch(error) {
      case SQLITE_OK:
          return NPT_SUCCESS;

      case SQLITE_BUSY:
          return OZN_ERROR_BUSY;

      default:
          return NPT_FAILURE;
    }
}

/*----------------------------------------------------------------------
|   OZN_Database::OZN_Database
+---------------------------------------------------------------------*/
OZN_Database::OZN_Database(sqlite3* m_Sq3Db, NPT_Int32 busy_timeout_ms) :
    m_Sq3Db(m_Sq3Db),
    m_BusyTimeoutMs(busy_timeout_ms)
{
}

/*----------------------------------------------------------------------
|   OZN_Database::~OZN_Database
+---------------------------------------------------------------------*/
OZN_Database::~OZN_Database(void)
{
    if (m_Sq3Db) {
        sqlite3_close(m_Sq3Db);
    }
}

/*----------------------------------------------------------------------
|   OZN_Database::Create
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Database::Create(const char*    path, 
                     NPT_Int32      busy_timeout_ms, 
                     bool           create_file,
                     OZN_Database*& db)
{
    int         result;
    sqlite3*    m_Sq3Db;

    // reset output params
    db = NULL;

    if (create_file) {
        NPT_FileInfo info;
        result = NPT_File::GetInfo(path, &info);
        if (NPT_FAILED(result)) {
            NPT_File file(path);
            NPT_CHECK(file.Open(NPT_FILE_OPEN_MODE_CREATE | NPT_FILE_OPEN_MODE_WRITE | NPT_FILE_OPEN_MODE_TRUNCATE));
            file.Close();

            // try to get type again
            NPT_CHECK(NPT_File::GetInfo(path, &info));
        }

        // check that this is a file 
        if (info.m_Type != NPT_FileInfo::FILE_TYPE_REGULAR) {
            NPT_Debug("The path %s does not point to a file, please check the path", path);
            return OZN_ERROR_DB_INVALID_FILEPATH;
        }        
    }

    result = sqlite3_open(path, &m_Sq3Db);
    if (result != SQLITE_OK) {
        //const char* errmsg = sqlite3_errmsg(m_Sq3Db);
        return OZN_MapErrorCode(result);
    }

    // create db now
    db = new OZN_Database(m_Sq3Db, busy_timeout_ms);

    // sets a custom busy handler
    sqlite3_busy_handler(m_Sq3Db, OZN_BusyHandler, db);

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_Database::ExecuteDML
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Database::ExecuteDML(const char* sql, NPT_UInt32* changes /* = NULL */)
{
    int result = result = sqlite3_exec(m_Sq3Db, sql, 0, 0, 0);

    switch(result) {
        case SQLITE_OK:
            if (changes) *changes = sqlite3_changes(m_Sq3Db);
            return NPT_SUCCESS;

        default:
            //const char* errmsg = sqlite3_errmsg(m_Sq3Db);
            return OZN_MapErrorCode(result);
    }
}

/*----------------------------------------------------------------------
|   OZN_Database::ExecuteScalar
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Database::ExecuteScalar(const char* sql, OZN_Property& property)
{
    NPT_Result result = NPT_FAILURE;
    OZN_Query* query;

    NPT_CHECK(OZN_Query::Create(this, sql, query));

    if (sqlite3_column_count((*query->m_Vm)()) < 1) goto failure;

    result = query->GetProperty(0, property);

failure:
    delete query;
    return result;
}

/*----------------------------------------------------------------------
|   OZN_Database::GetLastRowId
+---------------------------------------------------------------------*/
NPT_Result
OZN_Database::GetLastRowId(NPT_UInt32& id)
{
    id = (NPT_UInt32)sqlite3_last_insert_rowid(m_Sq3Db);
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_Database::Lock
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Database::Lock()
{
    return ExecuteDML("BEGIN EXCLUSIVE TRANSACTION lock", NULL);
}

/*----------------------------------------------------------------------
|   OZN_Database::Unlock
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Database::Unlock()
{
    return ExecuteDML("END TRANSACTION lock", NULL);
}


/*----------------------------------------------------------------------
|   OZN_Database::CheckTableSchema
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Database::CheckTableSchema(const OZN_TableDescription& desc) 
{
    NPT_Result          res = NPT_FAILURE;
    NPT_String          sql;
    NPT_String          sql_create;
    OZN_StringProperty  schema(0, "");
    const char*         result;

    // generate the sql statement we would use to create the table
    NPT_CHECK(OZN_Sql::CreateTable(desc, sql_create));

    // generate the sql statement to query for a table schema
    NPT_CHECK(OZN_Sql::GetTableSchema(desc.name, sql));

    // query the db, if the table doesn't exist it will fail
    res = ExecuteScalar(sql, schema);
    result = schema.GetValue().string;
    if (NPT_SUCCEEDED(res) && result && result[0] != '\0') {
        //if existing table schema sql matches the one we would use
        // then it is the same table and same schema version
        if (NPT_StringsEqual(result, sql_create)) {
            return NPT_SUCCESS;
        }

        // weird, the query succeeded but returned nothing
        return NPT_FAILURE;
    }

    // close bracket
    OZN_Sql::Close(sql_create);

    // table doesn't exist, create it
    NPT_CHECK(ExecuteDML(sql_create, NULL));

    if (desc.unique_index_ids_count && desc.unique_index_ids) {
        res = OZN_Sql::CreateUniqueIndex(desc, 
            desc.unique_index_ids,
            desc.unique_index_ids_count,
            sql);
        NPT_CHECK(res);

        // close bracket
        OZN_Sql::Close(sql);

        // create unique index
        NPT_CHECK(ExecuteDML(sql, NULL));
    }

    return NPT_SUCCESS;
}

