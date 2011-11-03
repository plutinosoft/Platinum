/*****************************************************************
|
|      Ozone - Schema Test Program 1
|
|   Copyright (c) 2004-2008, Plutinosoft, LLC.
|   Author: Sylvain Rebaud (sylvain@plutinosoft.com)
|
****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdarg.h>
#include "Ozone.h"

#if defined(WIN32) && defined(_DEBUG)
#include <crtdbg.h>
#endif

/*----------------------------------------------------------------------
|       macros
+---------------------------------------------------------------------*/
#define SHOULD_SUCCEED(r)                                   \
    do {                                                    \
        if (NPT_FAILED(r)) {                                \
            NPT_Debug("failed line %d (%d)\n", __LINE__, r);\
            exit(1);                                        \
        }                                                   \
    } while(0)                                         

#define SHOULD_FAIL(r)                                                  \
    do {                                                                \
        if (NPT_SUCCEEDED(r)) {                                         \
            NPT_Debug("should have failed line %d (%d)\n", __LINE__, r);\
            exit(1);                                                    \
        }                                                               \
    } while(0)                                  

#define SHOULD_EQUAL_I(a, b)                                           \
    do {                                                               \
        if ((a) != (b)) {                                              \
            NPT_Debug("got %l, expected %l line %d\n", a, b, __LINE__);\
            exit(1);                                                   \
        }                                                              \
    } while(0)                                  

#define SHOULD_EQUAL_F(a, b)                                           \
    do {                                                               \
        if ((a) != (b)) {                                              \
            NPT_Debug("got %f, expected %f line %d\n", a, b, __LINE__);\
            exit(1);                                                   \
        }                                                              \
    } while(0)                                  

#define SHOULD_EQUAL_S(a, b)                                           \
    do {                                                               \
        if (!NPT_StringsEqual(a,b)) {                                  \
            NPT_Debug("got %s, expected %s line %d\n", a, b, __LINE__);\
            exit(1);                                                   \
        }                                                              \
    } while(0)     

#define SHOULD_EQUAL_SN(a, b, n)                                       \
    do {                                                               \
        if (!NPT_StringsEqualN(a,b,n)) {                               \
            NPT_Debug("got %s, expected %s line %d\n", a, b, __LINE__);\
            exit(1);                                                   \
        }                                                              \
    } while(0)  

#define SHOULD_EQUAL_TRUE(b)                                           \
    do {                                                               \
        if (!(b)) {                                                    \
            NPT_Debug("got false, expected true line %d\n", __LINE__); \
            exit(1);                                                   \
        }                                                              \
    } while(0)

/*----------------------------------------------------------------------
|   constants
+---------------------------------------------------------------------*/
#define DB_PATH "test.db"

/*----------------------------------------------------------------------
|   typedefs
+---------------------------------------------------------------------*/
// column ids
typedef enum {
    OZN_COLUMN_ID,
    OZN_COLUMN_TYPE,
    OZN_COLUMN_NAME,
    OZN_COLUMN_VALUE,
    OZN_COLUMN_MAX
} OZN_ColumnId;

const OZN_PropertyDescription table1_property_descs[] = {
    {"Id",    OZN_PROPERTY_TYPE_STRING,  true, NULL},
    {"Type",  OZN_PROPERTY_TYPE_INTEGER, true, NULL},
    {"Name",  OZN_PROPERTY_TYPE_STRING,  true, NULL},
    {"Value", OZN_PROPERTY_TYPE_RAW,     true, NULL}
};

const OZN_TableDescription table1_description = {
    "table1",
    false,
    table1_property_descs,
    sizeof(table1_description.property_descs)/sizeof(OZN_PropertyDescription),
    NULL,
    0
};

/*----------------------------------------------------------------------
|   Test0
+---------------------------------------------------------------------*/
static void
Test0()
{
    NPT_Debug("--- Test0 Start ---\n");

    OZN_Properties properties;

    /* create empty properties from table description */
    SHOULD_SUCCEED(properties.CreateEmpty(table1_description));
    SHOULD_EQUAL_TRUE(properties.GetProperty(0)->GetType() == OZN_PROPERTY_TYPE_STRING);
    SHOULD_EQUAL_TRUE(properties.GetProperty(1)->GetType() == OZN_PROPERTY_TYPE_INTEGER);
    SHOULD_EQUAL_TRUE(properties.GetProperty(2)->GetType() == OZN_PROPERTY_TYPE_STRING);
    SHOULD_EQUAL_TRUE(properties.GetProperty(3)->GetType() == OZN_PROPERTY_TYPE_RAW);

    /* create properties from table description */
    SHOULD_SUCCEED(properties.Create(&table1_description, "Id0", 1, "Name1", "1234", 4));
    SHOULD_EQUAL_TRUE(properties.GetProperty(0)->GetType() == OZN_PROPERTY_TYPE_STRING);
    SHOULD_EQUAL_TRUE(properties.GetProperty(1)->GetType() == OZN_PROPERTY_TYPE_INTEGER);
    SHOULD_EQUAL_TRUE(properties.GetProperty(2)->GetType() == OZN_PROPERTY_TYPE_STRING);
    SHOULD_EQUAL_TRUE(properties.GetProperty(3)->GetType() == OZN_PROPERTY_TYPE_RAW);
    SHOULD_EQUAL_S(properties.GetProperty(0)->GetValue().string, "Id0");
    SHOULD_EQUAL_I(properties.GetProperty(1)->GetValue().integer, 1);
    SHOULD_EQUAL_S(properties.GetProperty(2)->GetValue().string, "Name1");
    SHOULD_EQUAL_I(properties.GetProperty(3)->GetValue().raw_data.size, 4);
    SHOULD_EQUAL_SN((const char*)properties.GetProperty(3)->GetValue().raw_data.data, "1234", 4);

    /* create properties from format string */
    SHOULD_SUCCEED(properties.Create("sdsr", "Id0", 1, "Name1", "1234", 4));
    SHOULD_EQUAL_I(properties.GetProperties().GetItemCount(), 4);
    SHOULD_EQUAL_TRUE(properties.GetProperty(0)->GetType() == OZN_PROPERTY_TYPE_STRING);
    SHOULD_EQUAL_TRUE(properties.GetProperty(1)->GetType() == OZN_PROPERTY_TYPE_INTEGER);
    SHOULD_EQUAL_TRUE(properties.GetProperty(2)->GetType() == OZN_PROPERTY_TYPE_STRING);
    SHOULD_EQUAL_TRUE(properties.GetProperty(3)->GetType() == OZN_PROPERTY_TYPE_RAW);

    /* can set existing property if matching type */
    SHOULD_SUCCEED(properties.SetProperty(0, "blah"));
    SHOULD_EQUAL_S(properties.GetProperty(0)->GetValue().string, "blah");
    SHOULD_SUCCEED(properties.SetProperty(1, 23));
    SHOULD_EQUAL_I(properties.GetProperty(1)->GetValue().integer, 23);
    SHOULD_SUCCEED(properties.SetProperty(3, "treehouse", 9));
    SHOULD_EQUAL_SN((const char*)properties.GetProperty(3)->GetValue().raw_data.data, "treehouse", 9);
    SHOULD_EQUAL_I(properties.GetProperty(3)->GetValue().raw_data.size, 9);

    SHOULD_SUCCEED(properties.SetProperty(3, NULL, 0));
    SHOULD_EQUAL_I(properties.GetProperty(3)->GetValue().raw_data.data, NULL);
    SHOULD_EQUAL_I(properties.GetProperty(3)->GetValue().raw_data.size, 0);

    /* cannot set a property with a different type if existing */
    SHOULD_FAIL(properties.SetProperty(0, 3));
    SHOULD_FAIL(properties.SetProperty(0, "232", 3));
    SHOULD_FAIL(properties.SetProperty(1, "3"));
    SHOULD_FAIL(properties.SetProperty(1, "232", 3));
    SHOULD_FAIL(properties.SetProperty(3, "3"));
    SHOULD_FAIL(properties.SetProperty(3, 4));
}

/*----------------------------------------------------------------------
|   Test1
+---------------------------------------------------------------------*/
static void
Test1()
{
    OZN_Database* db = NULL;

    NPT_Debug("--- Test1 Start ---\n");

    NPT_Debug("-- Creating Db ... --\n");
    SHOULD_SUCCEED(OZN_Database::Create(DB_PATH, 1000, true, db));

    // turning off synchronous for faster access
    NPT_Debug("-- Executing some DML on db ... --\n");
    SHOULD_SUCCEED(db->ExecuteDML("PRAGMA synchronous=OFF", NULL));
    SHOULD_SUCCEED(db->ExecuteDML("PRAGMA temp_store=MEMORY", NULL));

    // check/create table1 table
    NPT_Debug("-- Creating/checking table schema ... --\n");
    SHOULD_SUCCEED(db->CheckTableSchema(table1_description));   

    // try to insert
    OZN_Properties properties;
    OZN_Accessor*  accessor = new OZN_Accessor(db, table1_description);

    NPT_Debug("-- Calling GetObject on non existing entry ... --\n");
    SHOULD_EQUAL_I(accessor->Get("blah", properties), NPT_ERROR_NO_SUCH_ITEM);
    
    /* try to insert a row with invalid type */
    SHOULD_SUCCEED(properties.Create("sddr", "Id0", 1, 2, "1234", 4));
    SHOULD_FAIL(accessor->Put("Id0", properties));

    NPT_Debug("-- Calling PutObject ... --\n");
    SHOULD_SUCCEED(properties.Create(&table1_description, "Id0", 1, "Name1", "1234", 4));
    SHOULD_SUCCEED(accessor->Put("Id0", properties));

    NPT_Debug("-- Calling GetObject on existing entry ... --\n");
    SHOULD_SUCCEED(properties.CreateEmpty(table1_description));
    SHOULD_SUCCEED(accessor->Get("Id0", properties));
    SHOULD_EQUAL_TRUE(properties.GetProperty(0)->GetType() == OZN_PROPERTY_TYPE_STRING);
    SHOULD_EQUAL_TRUE(properties.GetProperty(1)->GetType() == OZN_PROPERTY_TYPE_INTEGER);
    SHOULD_EQUAL_TRUE(properties.GetProperty(2)->GetType() == OZN_PROPERTY_TYPE_STRING);
    SHOULD_EQUAL_TRUE(properties.GetProperty(3)->GetType() == OZN_PROPERTY_TYPE_RAW);
    SHOULD_EQUAL_S(properties.GetProperty(0)->GetValue().string, "Id0");
    SHOULD_EQUAL_I(properties.GetProperty(1)->GetValue().integer, 1);
    SHOULD_EQUAL_S(properties.GetProperty(2)->GetValue().string, "Name1");
    SHOULD_EQUAL_I(properties.GetProperty(3)->GetValue().raw_data.size, 4);
    SHOULD_EQUAL_SN((const char*)properties.GetProperty(3)->GetValue().raw_data.data, "1234", 4);

    NPT_Debug("-- Calling PutObject on existing entry... --\n");
    properties.SetProperty(1, 12);
    properties.SetProperty(2, "NewName1");
    properties.SetProperty(3, "newdata", 7);
    SHOULD_SUCCEED(accessor->Put("Id0", properties));
    SHOULD_EQUAL_I(properties.GetProperty(1)->GetValue().integer, 12);
    SHOULD_EQUAL_S(properties.GetProperty(2)->GetValue().string, "NewName1");
    SHOULD_EQUAL_I(properties.GetProperty(3)->GetValue().raw_data.size, 7);
    SHOULD_EQUAL_SN((const char*)properties.GetProperty(3)->GetValue().raw_data.data, "newdata", 7);

    delete accessor;
    delete db;
}

/*----------------------------------------------------------------------
|   Test2
+---------------------------------------------------------------------*/
static void
Test2()
{
    OZN_Database* db = NULL;
    NPT_Debug("--- Test1 Start ---\n");

    NPT_Debug("-- Creating Db ... --\n");
    SHOULD_SUCCEED(OZN_Database::Create(DB_PATH, 1000, true, db));


    delete db;
}

#if defined(WIN32) && defined(_DEBUG)
static int AllocHook(int allocType, void *userData, size_t size, int blockType, 
                     long requestNumber, const unsigned char *filename, int lineNumber)
{
    (void)allocType;
    (void)userData;
    (void)size;
    (void)blockType;
    (void)requestNumber;
    (void)lineNumber;
    (void)filename;
    return 1;
}
#endif

/*----------------------------------------------------------------------
|   main
+---------------------------------------------------------------------*/
int
main(int argc, char** argv) 
{
    NPT_COMPILER_UNUSED(argc);
    NPT_COMPILER_UNUSED(argv);

#if defined(WIN32) && defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF    |
                   _CRTDBG_CHECK_ALWAYS_DF |
                   _CRTDBG_LEAK_CHECK_DF);
    _CrtSetAllocHook(AllocHook);
#endif

    Test0();
    Test1();

    NPT_Debug("- program done -\n");

    return 0;
}






