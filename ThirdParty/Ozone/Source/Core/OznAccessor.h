/*****************************************************************
|
|   Ozone - Accessor
|
|   Copyright (c) 2004-2008, Plutinosoft, LLC.
|   Author: Sylvain Rebaud (sylvain@plutinosoft.com)
|
****************************************************************/

#ifndef _OZN_ACCESSOR_H_
#define _OZN_ACCESSOR_H_

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "Neptune.h"
#include "OznResults.h"
#include "OznProperty.h"
#include "OznDatabase.h"

/*----------------------------------------------------------------------
|   forward declarations
+---------------------------------------------------------------------*/

/*----------------------------------------------------------------------
|   OZN_Accessor class
+---------------------------------------------------------------------*/
class OZN_Accessor
{
public:
    OZN_Accessor(OZN_Database*               db,
                 const OZN_TableDescription& table_desc);
    ~OZN_Accessor();

    // accessor methods
    NPT_Result Get(const char*           primary_key, 
                   const OZN_Properties& properties);
    NPT_Result Put(const char*           primary_key, 
                   const OZN_Properties& properties);
    NPT_Result Delete(const char* primary_key);
    NPT_Result Delete(const OZN_Properties* properties = NULL);

private:
    // methods
    NPT_Result Delete(const OZN_Properties* properties = NULL,
                      NPT_UInt32*           changes = NULL);

private:
    OZN_Database*               m_Db;
    const OZN_TableDescription& m_TableDesc;
};


#endif /* _OZN_ACCESSOR_H_ */