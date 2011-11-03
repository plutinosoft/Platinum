/*****************************************************************
|
|   Ozone - Query
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
|   OZN_Query::OZN_Query
+---------------------------------------------------------------------*/
OZN_Query::OZN_Query(OZN_Database*          db,
                     NPT_Reference<OZN_VM>& vm,  
                     bool                   eof) :
    m_Db(db),
    m_Vm(vm),
    m_Eof(eof)
{
}

/*----------------------------------------------------------------------
|   OZN_Query::~OZN_Query
+---------------------------------------------------------------------*/
OZN_Query::~OZN_Query()
{
}

/*----------------------------------------------------------------------
|   OZN_Query::Create
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Query::Create(OZN_Database* db, 
                  const char*   sql,
                  OZN_Query*&   query)
{
    int                   result;
    sqlite3_stmt*         sq3_vm;
    const char*           tail = 0;
    NPT_Reference<OZN_VM> vm;

    // reset output params
    query = NULL;

    // check input params
    if (!db) return NPT_FAILURE;

    result = sqlite3_prepare(db->m_Sq3Db, sql, -1, &sq3_vm, &tail);
    if (result != SQLITE_OK) goto failure;

    result = sqlite3_step(sq3_vm);
    switch(result) {
        case SQLITE_ROW:
        case SQLITE_DONE:
            break;

        default:
            goto failure;
    }

    // create object now
    vm = new OZN_VM(sq3_vm);
    query = new OZN_Query(db, vm, (result == SQLITE_DONE));
    return NPT_SUCCESS;

failure:
    //const char* errmsg = sqlite3_errmsg(db->m_Sq3Db);
    return OZN_MapErrorCode(result);
}

/*----------------------------------------------------------------------
|   OZN_Query::GetProperty
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Query::GetProperty(NPT_UInt32    column,
                       OZN_Property& property)
{
    NPT_Result result;

    switch(property.GetType()) {
        case OZN_PROPERTY_TYPE_INTEGER: {
            int value = sqlite3_column_int((*m_Vm)(), column);
            result = property.SetValue((const OZN_PropertyValue*)&value);
            break;
        }

        case OZN_PROPERTY_TYPE_STRING: {
            const unsigned char* value = sqlite3_column_text((*m_Vm)(), column);
            result = property.SetValue((const OZN_PropertyValue*)(void*)&value);
            break;
        }

        case OZN_PROPERTY_TYPE_RAW: {
            OZN_RawData data;
            data.size = sqlite3_column_bytes((*m_Vm)(), column);
            data.data = sqlite3_column_blob((*m_Vm)(), column);
            result = property.SetValue((const OZN_PropertyValue*)(void*)&data);
            break;
        }

        default:
            return OZN_ERROR_INVALID_PROPERTY;
    }

    return result;
}

/*----------------------------------------------------------------------
|   OZN_Query::GetNext
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Query::GetNext(const OZN_TableDescription& desc,
                   const OZN_Properties&       properties)
{
    NPT_Result    result;
    int           i, j;
    NPT_Cardinal  props_found = 0;
    OZN_Property* property;
    int           column_count;

    if (m_Eof) return OZN_ERROR_NO_MORE_ITEMS;

    // retrieve number of columns in results 
    column_count = sqlite3_column_count((*m_Vm)());

    for (i=0; i<(int)desc.property_descs_count; i++) {
        // look for property name in table description
        property = properties.GetProperty(i);
        if (!property) continue;

        ++props_found;

        // verify type
        if (desc.property_descs[i].type != property->GetType())
            return OZN_ERROR_INVALID_PROPERTY;

        // look for column index from property name
        for (j=0; j<column_count; j++) {
            const char* column_name = sqlite3_column_name((*m_Vm)(), j);
            if (NPT_StringsEqual(column_name, desc.property_descs[i].name)) 
                break;
        }

        // the property name was not found our results!
        if (j == column_count) return OZN_ERROR_INVALID_PROPERTY;

        result = GetProperty(j, *property);       
        if (NPT_FAILED(result)) return result;
    }

    // did we find all the properties we were looking for?
    if (props_found != properties.GetProperties().GetItemCount()) 
        return NPT_FAILURE;

    // retrieve next entry
    result = sqlite3_step((*m_Vm)());
    switch(result) {
        case SQLITE_ROW:
        case SQLITE_DONE:
            break;

        default:
            //const char* errmsg = sqlite3_errmsg(m_Db->m_Sq3Db);
            return OZN_MapErrorCode(result);
    }

    m_Eof = (result == SQLITE_DONE);
    return NPT_SUCCESS;
}
