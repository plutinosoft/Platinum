/*****************************************************************
|
|   Ozone - Accessor
|
|   Copyright (c) 2004-2008, Plutinosoft, LLC.
|   Author: Sylvain Rebaud (sylvain@plutinosoft.com)
|
****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "OznAccessor.h"
#include "OznIterator.h"
#include "OznSql.h"
#include "OznStatement.h"

/*----------------------------------------------------------------------
|   constants
+---------------------------------------------------------------------*/

/*----------------------------------------------------------------------
|   OZN_Accessor::OZN_Accessor
+---------------------------------------------------------------------*/
OZN_Accessor::OZN_Accessor(OZN_Database*               db,
                           const OZN_TableDescription& table_desc) :
    m_Db(db),
    m_TableDesc(table_desc)
{
}

/*----------------------------------------------------------------------
|   OZN_Accessor::~OZN_Accessor
+---------------------------------------------------------------------*/
OZN_Accessor::~OZN_Accessor(void)
{
}

/*----------------------------------------------------------------------
|   OZN_Accessor::Get
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Accessor::Get(const char*           primary_key, 
                  const OZN_Properties& properties)
{
    OZN_Iterator*       iterator = NULL;
    NPT_Cardinal        num_rows;
    NPT_Result          result;
    OZN_Properties      properties_iterator;

    if (!primary_key) return NPT_ERROR_INVALID_PARAMETERS;

    // use an iterator with the only the primary key as property
    properties_iterator.SetProperty(0, primary_key);

    result = OZN_Iterator::Create(m_Db, 
        m_TableDesc,
        &properties_iterator,
        iterator);
    NPT_CHECK_LABEL(result, failure);

    // verify entry was found
    num_rows = iterator->GetNumRows();
    if (num_rows == 0) {
        result = NPT_ERROR_NO_SUCH_ITEM;
        goto failure;
    }

    // retrieve properties
    result = iterator->GetNext(properties);

failure:
    delete iterator;
    return result;
}

/*----------------------------------------------------------------------
|   OZN_Accessor::Put
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Accessor::Put(const char*           primary_key, 
                  const OZN_Properties& properties)
{
    OZN_Statement* statement = NULL;
    NPT_String     sql;
    NPT_Ordinal    i;
    NPT_Result     result;
    OZN_Property*  property;
    OZN_Property*  property_key_out = NULL;
    bool           is_update = false;
    NPT_Cardinal   placeholder_index;
    NPT_UInt32     changes;

    // figure out if it's an insert or update
    if (!primary_key) {
        // cannot pass no primary key to a
        // non auto-assigned table
        if (!m_TableDesc.auto_assign) {
            return OZN_ERROR_INVALID_PROPERTY;
        } else {
            // passing no primary key on an auto-assigned table
            // means we want to insert a new entry
            result = OZN_Sql::Insert(m_TableDesc, 
                properties,
                false,
                sql);
        }
    } else {
        // Passing a empty string for primary key is never allowed
        if (NPT_StringLength(primary_key) == 0) 
            return NPT_ERROR_INVALID_PARAMETERS;
            
        if (m_TableDesc.auto_assign) {
            // passing a primary key on an auto-assigned table
            // means we want to update an existing entry and fail
            // if the entry is not found
            result = OZN_Sql::Update(m_TableDesc, 
                properties,
                sql);
            is_update = true;
        } else {
            // passing a primary key on an non auto-assigned table
            // means we want to update an existing entry or insert
            // if not found
            result = OZN_Sql::Insert(m_TableDesc, 
                properties, 
                true,
                sql);
        }
    }
    NPT_CHECK_LABEL(result, failure);

    // close sql statement
    OZN_Sql::Close(sql);

    // see if the list of properties contains the property key
    property_key_out = properties.GetProperty(0);

    // create statement for it
    result = OZN_Statement::Create(
        m_Db,
        sql, 
        statement);
    NPT_CHECK_LABEL(result, failure);
    
    // bind key first 
    // if it's an update then bind to the last value 
    // else first index is key 
    placeholder_index = 1+(is_update?(properties.GetProperties().GetItemCount()-(property_key_out?1:0)):0);

    // if table is auto assigned, override property type to INTEGER
    result = statement->BindValue(
        placeholder_index,
        m_TableDesc.auto_assign?OZN_PROPERTY_TYPE_INTEGER:m_TableDesc.property_descs[0].type,
        primary_key);
    NPT_CHECK_LABEL(result, failure);

    // bind remaining properties
    placeholder_index = is_update?1:2;
    for (i=1; i<m_TableDesc.property_descs_count; i++) {
        property = properties.GetProperty(i);
        if (!property) continue;

        result = statement->Bind( placeholder_index++, property);
        NPT_CHECK_LABEL(result, failure);
    }

    result = statement->ExecuteDML(&changes);
    NPT_CHECK_LABEL(result, failure);

    // verify changes were properly made
    // in the case of a non existing entry update
    // it would succeed but returns 0 in changes
    if (changes < 1) {
        result = OZN_FAILURE;
        goto failure;
    }

    // update output property key
    if (property_key_out) {
        NPT_String last_row_id;
        const char* primary_key_out = primary_key;
        if (!primary_key) {
            // get db auto assigned id
            NPT_UInt32 id;
            m_Db->GetLastRowId(id);
            last_row_id = NPT_String::FromInteger(id);
            primary_key_out = last_row_id;
        }

        // TODO: we assume property key is always a string for now
        //  we should convert to int if necessary
        if (property_key_out->GetType() != OZN_PROPERTY_TYPE_STRING) {
            result = OZN_ERROR_INVALID_PROPERTY;
            goto failure;
        }

        result = property_key_out->SetValue((const OZN_PropertyValue*)&primary_key_out);
    }

failure:
    delete statement;
    return result;
}

/*----------------------------------------------------------------------
|   OZN_Accessor::Delete
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Accessor::Delete(const OZN_Properties* properties, 
                     NPT_UInt32*           changes)
{
    OZN_Statement* statement = NULL;
    NPT_String     sql;
    NPT_Result     result;

    // create sql statement
    result = OZN_Sql::Delete(
        m_TableDesc, 
        sql,
        properties);
    NPT_CHECK_LABEL(result, failure);

    // close sql statement
    OZN_Sql::Close(sql);

    // create statement for it
    result = OZN_Statement::Create(
        m_Db,
        sql,
        statement);
    NPT_CHECK_LABEL(result, failure);

    // bind if necessary
    if (properties) {
        result = statement->Bind(
            m_TableDesc,
            *properties);
        NPT_CHECK_LABEL(result, failure);
    }

    // execute statement and get back a query object
    result = statement->ExecuteDML(changes);

failure:
    delete statement;
    return result;
}

/*----------------------------------------------------------------------
|   OZN_Accessor::Delete
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Accessor::Delete(const char* primary_key)
{
    NPT_Result     result;
    NPT_UInt32     changes;
    OZN_Properties properties;


    properties.SetProperty(0, primary_key);

    // invoke delete with property_key only
    result = Delete(&properties, &changes);
    NPT_CHECK_LABEL(result, failure);

    // make sure changes were made to the db
    if (changes != 1) {
        result = NPT_ERROR_NO_SUCH_ITEM;
    }

failure:
    return result;
}

/*----------------------------------------------------------------------
|   OZN_Accessor::Delete
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Accessor::Delete(const OZN_Properties* properties)
{
    NPT_UInt32 changes;
    NPT_CHECK(Delete(properties, &changes));
    
    // sqlite implements the "DELETE FROM tablename" by deleting the table
    // and recreating it. Thus, when deleting an entire table, 
    // calling sqlite3_num_changes will return 0 even when there was data in 
    // the table. To go around this problem we could use the statement 
    // "DELETE FROM tablename WHERE 1" instead but we would get a bit hit
    // when the table is big. Instead, we check that changes were made to 
    // the table only when a WHERE statement was included (when count was > 0)
    if (properties && properties->GetProperties().GetItemCount() && changes < 1) {
        return NPT_ERROR_NO_SUCH_ITEM;
    }

    return NPT_SUCCESS;
}
