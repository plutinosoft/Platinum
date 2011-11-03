/*****************************************************************
|
|   Ozone - Iterator
|
|   Copyright (c) 2004-2008, Plutinosoft, LLC.
|   Author: Sylvain Rebaud (sylvain@plutinosoft.com)
|
****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "OznIterator.h"
#include "OznSql.h"
#include "OznQuery.h"

/*----------------------------------------------------------------------
|   constants
+---------------------------------------------------------------------*/

/*----------------------------------------------------------------------
|   OZN_Iterator::OZN_Iterator
+---------------------------------------------------------------------*/
OZN_Iterator::OZN_Iterator(OZN_Database*               db,
                           const OZN_TableDescription& table_desc,
                           OZN_Query*                  query,
                           NPT_Cardinal                num_rows) :
    m_Db(db),
    m_TableDesc(table_desc),
    m_Query(query),
    m_NumRows(num_rows)
{
}

/*----------------------------------------------------------------------
|   OZN_Iterator::~OZN_Iterator
+---------------------------------------------------------------------*/
OZN_Iterator::~OZN_Iterator(void)
{
    delete m_Query;
}

/*----------------------------------------------------------------------
|   OZN_Iterator::Create
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Iterator::Create(OZN_Database*               db, 
                     const OZN_TableDescription& table_desc,
                     const OZN_Properties*       properties, 
                     OZN_Iterator*&              iterator)
{
    NPT_Result        result;
    OZN_Statement*    statement_count = NULL;
    OZN_Query*        query_count = NULL;
    OZN_IntProperty   property_count(0, 0);
    OZN_Statement*    statement = NULL;
    OZN_Query*        query = NULL;
    NPT_String        sql;

    // reset output params first
    iterator = NULL;

    // create sql statement to count rows first
    result = OZN_Sql::Select(table_desc, sql, true, properties);
    NPT_CHECK_LABEL(result, failure);

    // close statement
    OZN_Sql::Close(sql);

    // create statement for it
    result = OZN_Statement::Create(db,
        sql,
        statement_count);
    NPT_CHECK_LABEL(result, failure);

    // bind with properties now
    if (properties) {
        result = statement_count->Bind(table_desc, *properties);
        NPT_CHECK_LABEL(result, failure);
    }

    // execute statement and get back a query object
    result = statement_count->ExecuteQuery(query_count);
    NPT_CHECK_LABEL(result, failure);

    /* retrieve count */
    result = query_count->GetProperty(0, property_count);
    NPT_CHECK_LABEL(result, failure);

    // create statement for real query now
    result = OZN_Sql::Select(table_desc, sql, false, properties);
    NPT_CHECK_LABEL(result, failure);

    // close statement
    OZN_Sql::Close(sql);

    // create statement for it
    result = OZN_Statement::Create(db,
        sql, 
        statement);
    NPT_CHECK_LABEL(result, failure);

    // bind with properties now
    if (properties) {
        result = statement->Bind(table_desc, *properties);
        NPT_CHECK_LABEL(result, failure);
    }

    // execute statement and get back a query object
    result = statement->ExecuteQuery(query);
    NPT_CHECK_LABEL(result, failure);

    // do we need to do this?
    //if (property_count.GetValue().integer == 0) {
    //    query->m_Eof = true;
    //}

    // create object now
    iterator = new OZN_Iterator(db, 
        table_desc, 
        query, 
        property_count.GetValue().integer);

    // iterator has ownership of query now
    query = NULL;

    result = NPT_SUCCESS;

failure:
    delete statement_count;
    delete query_count;
    delete statement;
    delete query;
    return result;
}

/*----------------------------------------------------------------------
|   OZN_Iterator::GetNumRows
+---------------------------------------------------------------------*/
NPT_Cardinal 
OZN_Iterator::GetNumRows()
{
    return m_NumRows;
}

/*----------------------------------------------------------------------
|   OZN_Iterator::GetNext
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Iterator::GetNext(const OZN_Properties& properties)
{
    /* retrieved properties */
    return m_Query->GetNext(m_TableDesc, properties);
}