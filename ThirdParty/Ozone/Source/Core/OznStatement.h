/*****************************************************************
|
|   Ozone - Statement
|
|   Copyright (c) 2004-2008, Plutinosoft, LLC.
|   Author: Sylvain Rebaud (sylvain@plutinosoft.com)
|
****************************************************************/

#ifndef _OZN_STATEMENT_H_
#define _OZN_STATEMENT_H_

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "Neptune.h"
#include "sqlite3.h"
#include "OznResults.h"
#include "OznProperty.h"
#include "OznSchema.h"

/*----------------------------------------------------------------------
|   forward declarations
+---------------------------------------------------------------------*/
class OZN_Query;
class OZN_Database;

/*----------------------------------------------------------------------
|   OZN_VM class
+---------------------------------------------------------------------*/
class OZN_VM
{
public:
    OZN_VM(sqlite3_stmt* sq3_vm) : m_Sq3Vm(sq3_vm) {}
    ~OZN_VM() {
        if (m_Sq3Vm) sqlite3_finalize(m_Sq3Vm);
    }

    sqlite3_stmt* operator()(void) {
        return m_Sq3Vm;
    }

private:
    sqlite3_stmt* m_Sq3Vm;
};

/*----------------------------------------------------------------------
|   OZN_Statement class
+---------------------------------------------------------------------*/
class OZN_Statement
{
public:
    ~OZN_Statement();

    // class methods
    static NPT_Result Create(OZN_Database*   db, 
                             const char*     sql,
                             OZN_Statement*& statement);

    // methods
    NPT_Result Bind(const OZN_TableDescription& desc,
                    const OZN_Properties&       properties);
    NPT_Result Bind(NPT_Ordinal index, OZN_Property* property);
    NPT_Result BindValue(NPT_Ordinal      index,
                         OZN_PropertyType type,
                         const char*      value);    
    NPT_Result ExecuteQuery(OZN_Query*& query);
    NPT_Result ExecuteDML(NPT_UInt32* changes = NULL);
    NPT_Result Reset();

private:
    // methods
    OZN_Statement(OZN_Database* db, NPT_Reference<OZN_VM>& vm);


private:
    OZN_Database* m_Db;
    NPT_Reference<OZN_VM> m_Vm;
};

#endif /* _OZN_STATEMENT_H_ */