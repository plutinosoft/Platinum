/*****************************************************************
|
|   Ozone - Schema
|
|   Copyright (c) 2004-2008, Plutinosoft, LLC.
|   Author: Sylvain Rebaud (sylvain@plutinosoft.com)
|
****************************************************************/

#ifndef _OZN_SCHEMA_H_
#define _OZN_SCHEMA_H_

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "Neptune.h"
#include "OznProperty.h"

/*----------------------------------------------------------------------
|   typedefs
+---------------------------------------------------------------------*/
typedef struct OZN_PropertyDescription {
    const char*        name;
    OZN_PropertyType   type;
    bool               required;
    OZN_PropertyValue* default_value;
} OZN_PropertyDescription;

typedef NPT_UInt32 OZN_TableId;

typedef struct OZN_TableDescription {
    const char*                    name;
    bool                           auto_assign;
    const OZN_PropertyDescription* property_descs;
    NPT_Cardinal                   property_descs_count;
    const OZN_PropertyId*          unique_index_ids;
    NPT_Cardinal                   unique_index_ids_count;
} OZN_TableDescription;

#endif /* _OZN_SCHEMA_H_ */