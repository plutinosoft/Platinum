/*****************************************************************
|
|   Ozone - Sql syntax
|
|   Copyright (c) 2004-2008, Plutinosoft, LLC.
|   Author: Sylvain Rebaud (sylvain@plutinosoft.com)
|
****************************************************************/

#ifndef _OZN_SQL_H_
#define _OZN_SQL_H_

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "Neptune.h"
#include "OznProperty.h"
#include "OznSchema.h"

/*----------------------------------------------------------------------
|   OZN_Sql class
+---------------------------------------------------------------------*/
class OZN_Sql
{
public:
    static NPT_Result GetTableSchema(const char* table_name, NPT_String& sql);
    static NPT_Result CreateTable(const OZN_TableDescription& desc, NPT_String& sql);
    static NPT_Result CreateUniqueIndex(const OZN_TableDescription& desc,
                                        const OZN_PropertyId*       property_ids, 
                                        NPT_Cardinal                count,
                                        NPT_String&                 sql);
    static NPT_Result Insert(const OZN_TableDescription& desc,
                             const OZN_Properties&       properties,
                             bool                        replace_on_conflict, 
                             NPT_String&                 sql);
    static NPT_Result Update(const OZN_TableDescription& desc,
                             const OZN_Properties&       properties,
                             NPT_String&                 sql);
    static NPT_Result Select(const OZN_TableDescription& desc,
                             NPT_String&                 sql, 
                             bool                        select_is_count,
                             const OZN_Properties*       properties = NULL);
    static NPT_Result Delete(const OZN_TableDescription& desc,
                             NPT_String&                 sql,
                             const OZN_Properties*       properties = NULL);
    static NPT_Result AddCondition(NPT_String&      sql, 
                                   const char*      name,
                                   OZN_PropertyType type,
                                   const char*      value, 
                                   bool             first);
    static void Close(NPT_String& sql);
};

#endif /* _OZN_SQL_H_ */