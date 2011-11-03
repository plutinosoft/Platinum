/*****************************************************************
|
|   Ozone - Query
|
|   Copyright (c) 2004-2008, Plutinosoft, LLC.
|   Author: Sylvain Rebaud (sylvain@plutinosoft.com)
|
****************************************************************/

#ifndef _OZN_QUERY_H_
#define _OZN_QUERY_H_

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "Neptune.h"
#include "sqlite3.h"
#include "OznResults.h"
#include "OznProperty.h"
#include "OznSchema.h"
#include "OznStatement.h"

/*----------------------------------------------------------------------
|   OZN_Query class
+---------------------------------------------------------------------*/
class OZN_Query
{
    friend OZN_Statement;
    friend OZN_Database;

public:
    ~OZN_Query();

    // class methods
    static NPT_Result Create(OZN_Database* db, 
                             const char*   sql,
                             OZN_Query*&   query);

    // accessor methods
    NPT_Result GetProperty(NPT_UInt32    column,
                           OZN_Property& property);
    NPT_Result GetNext(const OZN_TableDescription& desc,
                       const OZN_Properties&       properties);

private:
    // methods
    OZN_Query(OZN_Database*          db, 
              NPT_Reference<OZN_VM>& vm,
              bool                   eof);

private:
    OZN_Database*         m_Db;
    NPT_Reference<OZN_VM> m_Vm;
    bool                  m_Eof;
};

#endif /* _OZN_DATABASE_H_ */