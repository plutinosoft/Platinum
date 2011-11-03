/*****************************************************************
|
|   Ozone - Sql syntax
|
|   Copyright (c) 2004-2008, Plutinosoft, LLC.
|   Author: Sylvain Rebaud (sylvain@plutinosoft.com)
|
****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "OznSql.h"

/*----------------------------------------------------------------------
|   OZN_Sql::GetTableSchema
+---------------------------------------------------------------------*/
// Example: 
// SELECT sql FROM sqlite_master WHERE tble_name='foo' AND type='table'

NPT_Result 
OZN_Sql::GetTableSchema(const char* table_name, NPT_String& sql) 
{
    // reset output params first
    sql = "";

    // verify input params
    if (!table_name) return NPT_ERROR_INVALID_PARAMETERS;

    sql  = "SELECT sql FROM sqlite_master WHERE tbl_name=\'";
    sql += table_name;
    sql += "\' AND type=\'table\'";

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_Sql::CreateTable
+---------------------------------------------------------------------*/
// Example:
// CREATE TABLE bar ( column1 INTEGER PRIMARY KEY,     column2 TEXT NOT NULL)
// CREATE TABLE foo ( column1 INTEGER NOT NULL UNIQUE, column2 BLOB NOT NULL DEFAULT X'')
// CREATE TABLE fo2 ( column1 TEXT NOT NULL UNIQUE,    column2 INTEGER NOT NULL DEFAULT '0')
// CREATE TABLE fo3 ( column1 BLOB NOT NULL UNIQUE,    column2 INTEGER NOT NULL DEFAULT '0')

NPT_Result 
OZN_Sql::CreateTable(const OZN_TableDescription& desc, NPT_String& sql)
{
    NPT_Cardinal i;

    // reset output params first
    sql = "";

    // verify table description
    if (desc.property_descs_count < 2) {
        return NPT_ERROR_INVALID_PARAMETERS;
    }

    // verify that first column is always of type string for our implementation
    if (desc.property_descs[0].type != OZN_PROPERTY_TYPE_STRING)
        return NPT_FAILURE;

    // start sql statement
    sql  = "CREATE TABLE ";
    sql += desc.name;
    sql += " (";

    for (i=0; i<desc.property_descs_count; i++) {
        // column name
        sql += " ";
        sql += desc.property_descs[i].name;


        // column type
        if (i==0 && desc.auto_assign) {
            sql += " INTEGER PRIMARY KEY ";
        } else {
            switch(desc.property_descs[i].type) {
                case OZN_PROPERTY_TYPE_INTEGER:
                    sql += " INTEGER";
                    break;
                case OZN_PROPERTY_TYPE_STRING:
                    sql += " TEXT";
                    break;
                case OZN_PROPERTY_TYPE_RAW:
                    sql += " BLOB";
                    break;
                default:
                    return NPT_FAILURE;
            }
        }

        // primary key?
        if (i==0) {
            if (!desc.auto_assign) {
                sql += " NOT NULL UNIQUE ";
            }
        } else {
            sql += " NOT NULL";
        }

        // default value
        if (!desc.property_descs[i].required) {
            sql += " DEFAULT ";

            // no default value found!
            if (!desc.property_descs[i].default_value) return NPT_FAILURE;

            switch(desc.property_descs[i].type) {
                case OZN_PROPERTY_TYPE_INTEGER: {
                        sql += "\'";
                        sql += NPT_String::FromInteger(desc.property_descs[i].default_value->integer);
                        sql += "\'";
                    }
                    break;
                case OZN_PROPERTY_TYPE_STRING:
                    sql += "\'";
                    sql += desc.property_descs[i].default_value->string;
                    sql += "\'";
                    break;
                case OZN_PROPERTY_TYPE_RAW:
                    sql += "X\'\'";
                    // empty string for raw for now
                    break;
                default:
                    return NPT_FAILURE;
            }
        }

        if (i<desc.property_descs_count-1) {
            sql += ", ";
        } else {
            sql += ")";
        }
    }
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_FindPropertyId
+---------------------------------------------------------------------*/
static NPT_Result
OZN_FindPropertyId(OZN_PropertyId        id, 
                   const OZN_PropertyId* ids, 
                   NPT_Cardinal          count)
{
    NPT_Cardinal i;

    for (i=0; i<count; i++) {
        if (id == ids[i]) {
            return NPT_SUCCESS;
        }
    }

    return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   OZN_Sql::CreateUniqueIndex
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Sql::CreateUniqueIndex(const OZN_TableDescription& desc,
                           const OZN_PropertyId*       property_ids, 
                           NPT_Cardinal                count,
                           NPT_String&                 sql)
{
    NPT_Cardinal i;
    NPT_Cardinal properties_count = 0;

    // reset output params first
    sql = "";

    // verify input params
    if (!property_ids || count < 1 || desc.property_descs_count < 2) 
        return NPT_ERROR_INVALID_PARAMETERS;

    // verify that we were not passed the primary key as it is already unique
    if (NPT_SUCCEEDED(OZN_FindPropertyId(0, property_ids, count))) {
        return NPT_ERROR_INVALID_PARAMETERS;
    }

    // start the sql statement
    sql  = "CREATE UNIQUE INDEX ";
    sql += desc.name;
    sql += "_index ON " ;
    sql += desc.name;
    sql += "(";


    // look for all other properties
    for (i=1; i<desc.property_descs_count; i++) {
        NPT_Result res = OZN_FindPropertyId(i, property_ids, count);
        if (NPT_FAILED(res)) {
            continue;
        }

        // add separator if needed before
        if (properties_count > 0) sql += ", ";

        // add column name 
        sql += desc.property_descs[i].name;
        ++properties_count;
    }

    // make sure all properties passed were accounted for
    if (properties_count != count) return NPT_FAILURE;

    sql += ")";

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_Sql::Insert
+---------------------------------------------------------------------*/
NPT_Result
OZN_Sql::Insert(const OZN_TableDescription& desc,
                const OZN_Properties&       properties, 
                bool                        replace_on_conflict, 
                NPT_String&                 sql)
{
    NPT_Cardinal  i;
    NPT_Cardinal  properties_count = 0;
    OZN_Property* property;

    // reset output params first
    sql = "";

    // verify input params
    if (desc.property_descs_count < 2) {
        return NPT_ERROR_INVALID_PARAMETERS;
    }

    // start sql statement
    if (replace_on_conflict) {
        sql  = "INSERT OR REPLACE INTO ";
    } else {
        sql  = "INSERT INTO ";
    }
    sql += desc.name;
    sql += " values (";

    // look for all properties other than the key
    for (i=1; i<desc.property_descs_count; i++) {
        property = properties.GetProperty(i);
        if (!property) {
            if (desc.property_descs[i].required) return OZN_ERROR_MISSING_PROPERTY;
            continue;
        }

        // verify type
        if (property->GetType() != desc.property_descs[i].type) {
            return OZN_ERROR_INVALID_PROPERTY;
        }

        // add separator
        if (properties_count>0) sql += ", ";

        // add placeholder
        sql += "?";
        ++properties_count;
    }

    // append placeholder for key
    sql += ", ?)";

    // account for key property
    if (property = properties.GetProperty(0)) {
        // verify type
        if (property->GetType() != desc.property_descs[0].type) {
            return OZN_ERROR_INVALID_PROPERTY;
        }
        ++properties_count;
    }

    // make sure all properties passed were accounted for
    if (properties_count != properties.GetProperties().GetItemCount()) {
        return NPT_FAILURE;
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_Sql::Update
+---------------------------------------------------------------------*/
NPT_Result
OZN_Sql::Update(const OZN_TableDescription& desc,
                const OZN_Properties&       properties,
                NPT_String&                 sql)
{
    NPT_Cardinal  i;
    OZN_Property* property;
    NPT_Cardinal  properties_count = 0;

    // reset output params first
    sql = "";

    // verify input params
    if (properties.GetProperties().GetItemCount() == 0 || desc.property_descs_count<2) {
        return NPT_ERROR_INVALID_PARAMETERS;
    }

    /* start sql statement */
    sql  = "UPDATE ";
    sql += desc.name;
    sql += " SET ";

    // append SETs
    for (i=1; i<desc.property_descs_count; i++) {
        property = properties.GetProperty(i);
        if (!property) continue;

        // verify type
        if (property->GetType() != desc.property_descs[i].type) {
            return OZN_ERROR_INVALID_PROPERTY;
        }

        // add separator
        if (properties_count>0) sql += ", ";

        // add placeholder SET
        sql += desc.property_descs[i].name;
        sql += " = ? ";
        ++properties_count;
    }

    // we should at least found one property to set
    if (properties_count == 0) return NPT_FAILURE;

    // append WHERE
    sql += " WHERE ";
    sql += desc.property_descs[0].name;
    sql += " = ? ";

    // account for key property
    if (property = properties.GetProperty(0)) {
        // verify type
        if (property->GetType() != desc.property_descs[0].type) {
            return OZN_ERROR_INVALID_PROPERTY;
        }
        ++properties_count;
    }

    // make sure all properties passed were accounted for
    if (properties_count != properties.GetProperties().GetItemCount()) {
        return NPT_FAILURE;
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_Sql::Select
+---------------------------------------------------------------------*/
NPT_Result
OZN_Sql::Select(const OZN_TableDescription& desc,
                NPT_String&                 sql, 
                bool                        select_is_count,
                const OZN_Properties*       properties)
{
    bool            first_condition = true;
    NPT_Cardinal    i;
    OZN_Property*   property;

    // reset output params first
    sql = "";

    // verify input params
    if (desc.property_descs_count<2) {
        return NPT_ERROR_INVALID_PARAMETERS;
    }

    // start sql statement
    if (select_is_count) {
        sql  = "SELECT count(*) FROM ";
    } else {
        sql  = "SELECT * FROM ";
    }
    sql += desc.name;

    if (properties) {
        // add late binding conditions
        for (i=0; i<desc.property_descs_count; i++) {
            property = properties->GetProperty(i);
            if (!property) continue;

            // append where condition with key and
            // specify NULL as we'll do a late binding
            NPT_CHECK(OZN_Sql::AddCondition(sql, 
                desc.property_descs[i].name,
                desc.property_descs[i].type,
                NULL,
                first_condition));

            first_condition = false;
        }
    }

    // append ORDER
    sql += " ORDER BY ";
    sql += desc.property_descs[0].name;
    sql += " ";

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_Sql::Delete
+---------------------------------------------------------------------*/
NPT_Result
OZN_Sql::Delete(const OZN_TableDescription& desc,
                NPT_String&                 sql,
                const OZN_Properties*       properties)
{
    bool          first_condition = true;
    NPT_Cardinal  i;
    OZN_Property* property;

    // reset output params first
    sql = "";

    // verify input params
    if (desc.property_descs_count<2) {
        return NPT_ERROR_INVALID_PARAMETERS;
    }

    // start sql statement
    sql  = "DELETE FROM ";
    sql += desc.name;

    if (properties) {
        /* add late binding conditions */
        for (i=0; i<desc.property_descs_count; i++) {
            property = properties->GetProperty(i);
            if (!property) continue;

            /**
            * append where condition with key and
            * specify NULL as we'll do a late binding
            */
            NPT_CHECK(OZN_Sql::AddCondition(sql, 
                desc.property_descs[i].name,
                desc.property_descs[i].type,
                NULL,
                first_condition));

            first_condition = false;
        }
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_Sql::AddCondition
+---------------------------------------------------------------------*/
NPT_Result
OZN_Sql::AddCondition(NPT_String&      sql, 
                      const char*      name,
                      OZN_PropertyType type,
                      const char*      value, 
                      bool             first)
{
    /* append WHERE */
    sql += first?" WHERE ":" AND ";
    sql += name;
    sql += " = ";

    /* special case used for statements with late binding */
    if (!value) {
        sql += "?";
    } else {
        switch(type) {
            case OZN_PROPERTY_TYPE_INTEGER: 
                sql += value;
                break;
            case OZN_PROPERTY_TYPE_STRING:{
                    sql += "\'";
                    sql += value;
                    sql += "\' ";
                }
                break;
            case OZN_PROPERTY_TYPE_RAW:
                // fallthrough
            default:
                return NPT_FAILURE;
        }
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_Sql::Close
+---------------------------------------------------------------------*/
void
OZN_Sql::Close(NPT_String& sql)
{
    sql += ";";
}
