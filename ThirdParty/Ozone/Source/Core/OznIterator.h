/*****************************************************************
|
|   Ozone - Iterator
|
|   Copyright (c) 2004-2008, Plutinosoft, LLC.
|   Author: Sylvain Rebaud (sylvain@plutinosoft.com)
|
****************************************************************/

#ifndef _OZN_ITERATOR_H_
#define _OZN_ITERATOR_H_

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "Neptune.h"
#include "OznResults.h"
#include "OznProperty.h"
#include "OznDatabase.h"
#include "OznSchema.h"

/*----------------------------------------------------------------------
|   forward declarations
+---------------------------------------------------------------------*/

/*----------------------------------------------------------------------
|   OZN_Iterator class
+---------------------------------------------------------------------*/
class OZN_Iterator
{
public:
    ~OZN_Iterator();

    // class methods
    static NPT_Result Create(OZN_Database*               database, 
                             const OZN_TableDescription& table_desc,
                             const OZN_Properties*       properties,
                             OZN_Iterator*&              iterator);

    // accessor methods
    NPT_Cardinal GetNumRows();
    NPT_Result   GetNext(const OZN_Properties& properties);

private:
    // methods
    OZN_Iterator(OZN_Database*               db,
                 const OZN_TableDescription& table_desc,
                 OZN_Query*                  query,
                 NPT_Cardinal                num_rows);

private:
    OZN_Database*               m_Db;
    const OZN_TableDescription& m_TableDesc;
    OZN_Query*                  m_Query;
    NPT_Cardinal                m_NumRows;
};


#endif /* _OZN_ITERATOR_H_ */