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
#include "OznDatabase.h"
#include "OznStatement.h"
#include "OznQuery.h"

/*----------------------------------------------------------------------
|   OZN_Statement::OZN_Statement
+---------------------------------------------------------------------*/
OZN_Statement::OZN_Statement(OZN_Database*          db,
                             NPT_Reference<OZN_VM>& vm) :
    m_Db(db),
    m_Vm(vm)
{
}

/*----------------------------------------------------------------------
|   OZN_Statement::~OZN_Statement
+---------------------------------------------------------------------*/
OZN_Statement::~OZN_Statement()
{
}

/*----------------------------------------------------------------------
|   OZN_Statement::Create
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Statement::Create(OZN_Database*   db, 
                      const char*     sql,
                      OZN_Statement*& statement)
{
    int           result;
    sqlite3_stmt* sq3_vm;
    const char*   tail = 0;

    // reset output params first
    statement = NULL;

    // check input params
    if (!db || !db->m_Sq3Db) return NPT_FAILURE;

    result = sqlite3_prepare(db->m_Sq3Db, sql, -1, &sq3_vm, &tail);
    if (result != SQLITE_OK) {
        //const char* errmsg = sqlite3_errmsg(db->m_Sq3Db);
        return OZN_MapErrorCode(result);
    }

    //create object now
    NPT_Reference<OZN_VM> vm(new OZN_VM(sq3_vm));
    statement = new OZN_Statement(db, vm);
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_Statement::Bind
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Statement::Bind(const OZN_TableDescription& desc,
                    const OZN_Properties&       properties)
{
    NPT_Result    result;
    NPT_Ordinal   bind_index = 1;
    OZN_Property* property;

    // verify input params
    if (properties.GetProperties().GetItemCount() == 0) 
        return NPT_ERROR_INVALID_PARAMETERS;

    // bind now
    for (NPT_Cardinal i=0; i<desc.property_descs_count; i++) {
        property = properties.GetProperty(i);
        if (!property) continue;

        if (i == 0) {
            // bind key, if table is auto assigned, override property type to INTEGER
            result = BindValue(
                bind_index++,
                desc.auto_assign?OZN_PROPERTY_TYPE_INTEGER:desc.property_descs[i].type,
                property->GetValue().string);
            NPT_CHECK(result);
        } else {
            result = Bind(
                bind_index++,
                property);
            NPT_CHECK(result);
        }
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_Statement::Bind
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Statement::Bind(NPT_Ordinal   index, OZN_Property* property)
{
    int         result;
    const char* errmsg;

    // special case ?
    if (!property) {
        result = sqlite3_bind_null((*m_Vm)(), index);
    } else {
        switch (property->GetType()) {
            case OZN_PROPERTY_TYPE_STRING: {
                result = sqlite3_bind_text((*m_Vm)(), 
                    index, 
                    property->GetValue().string, 
                    -1, 
                    SQLITE_TRANSIENT);
                break;
            }

            case OZN_PROPERTY_TYPE_INTEGER: {
                result = sqlite3_bind_int((*m_Vm)(), 
                    index, 
                    property->GetValue().integer);
                break;
            }

            case OZN_PROPERTY_TYPE_RAW: {
                const char* data = (const char*)property->GetValue().raw_data.data;
                result = sqlite3_bind_blob((*m_Vm)(), 
                    index,
                    (const void*)data?data:"", 
                    property->GetValue().raw_data.size, 
                    SQLITE_TRANSIENT);
                break;
            }

            default:
                return NPT_FAILURE;
        }
    }

    if (result != SQLITE_OK) {
        errmsg = sqlite3_errmsg(m_Db->m_Sq3Db);
    }

    return OZN_MapErrorCode(result);
}

/*----------------------------------------------------------------------
|   OZN_Statement::BindValue
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Statement::BindValue(NPT_Ordinal      index,
                         OZN_PropertyType type,
                         const char*      value)
{
    int result;

    if (!value) {
        return Bind(index, NULL);
    }

    switch(type) {
        case OZN_PROPERTY_TYPE_INTEGER: {
            NPT_Int32 data;
            NPT_CHECK(NPT_ParseInteger32(value, data, false));
            result = sqlite3_bind_int((*m_Vm)(), 
                index, 
                data);
            break;
        }        
        
        case OZN_PROPERTY_TYPE_STRING: {
            result = sqlite3_bind_text((*m_Vm)(), 
                index, 
                value, 
                -1, 
                SQLITE_TRANSIENT);
            break;
        }

        case OZN_PROPERTY_TYPE_RAW: {
            result = sqlite3_bind_blob((*m_Vm)(), 
                index,
                (const void*)value, 
                NPT_StringLength(value), 
                SQLITE_TRANSIENT);
            break;
        }

        default:
            return NPT_FAILURE;
    }

    if (result != SQLITE_OK) {
        const char* errmsg = sqlite3_errmsg(m_Db->m_Sq3Db);
    }

    return OZN_MapErrorCode(result);
}

/*----------------------------------------------------------------------
|   OZN_Statement::ExecuteQuery
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Statement::ExecuteQuery(OZN_Query*& query)
{
    int result = sqlite3_step((*m_Vm)());
    switch(result) {
        case SQLITE_ROW:
        case SQLITE_DONE:
            break;

        default:
            //const char* errmsg = sqlite3_errmsg(m_Db->m_Sq3Db);
            return OZN_MapErrorCode(result);
    }
    
    // vm is reference counted so it's ok to delete this
    // statement and only keep the returned query
    query = new OZN_Query(m_Db, m_Vm, (result == SQLITE_DONE));
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_Statement::ExecuteDML
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Statement::ExecuteDML(NPT_UInt32* changes /* = NULL */)
{
    int result = sqlite3_step((*m_Vm)());
    switch(result) {
        case SQLITE_DONE:
            if (changes) *changes = sqlite3_changes(m_Db->m_Sq3Db);
            break;

        default:
            //const char* errmsg = sqlite3_errmsg(m_Sq3Db);
            return OZN_MapErrorCode(result);
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_Statement::Reset
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Statement::Reset()
{
    int result = sqlite3_reset((*m_Vm)());
    if (result != SQLITE_OK) {
        //const char* errmsg = sqlite3_errmsg(statement->m_Sq3Db);
    }

    return OZN_MapErrorCode(result);
}

