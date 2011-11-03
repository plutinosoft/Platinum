/*****************************************************************
|
|   Ozone - Sqlite3 C++ interface
|
|   Copyright (c) 2004-2008, Plutinosoft, LLC.
|   Author: Sylvain Rebaud (sylvain@plutinosoft.com)
|
****************************************************************/

#ifndef _OZN_PROPERTY_H_
#define _OZN_PROPERTY_H_

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "Neptune.h"
#include "sqlite3.h"
#include "OznResults.h"

/*----------------------------------------------------------------------
|   forward declarations
+---------------------------------------------------------------------*/
struct OZN_TableDescription;

/*----------------------------------------------------------------------
|   typedefs
+---------------------------------------------------------------------*/
typedef NPT_UInt32 OZN_PropertyId;

typedef enum {
    OZN_PROPERTY_TYPE_INTEGER,
    OZN_PROPERTY_TYPE_STRING,
    OZN_PROPERTY_TYPE_RAW
} OZN_PropertyType;              

typedef struct {
    NPT_Size     size;
    NPT_AnyConst data;
} OZN_RawData;

typedef union {
    void*       any;
    NPT_Int32   integer;
    const char* string;
    OZN_RawData raw_data;
} OZN_PropertyValue;

/*----------------------------------------------------------------------
|   OZN_Property class
+---------------------------------------------------------------------*/
class OZN_Property
{
public:
    virtual ~OZN_Property() {}

    // accessor methods
    virtual OZN_PropertyId           GetId() { return m_Id; }

    // pure methods
    virtual OZN_PropertyType         GetType() = 0;
    virtual const OZN_PropertyValue& GetValue() = 0;
    virtual NPT_Result               SetValue(const OZN_PropertyValue* value) = 0;

protected:
    OZN_Property(OZN_PropertyId id) : m_Id(id) {}

private:
    OZN_PropertyId    m_Id;

protected:
    OZN_PropertyValue m_Value;
};

/*----------------------------------------------------------------------
|   OZN_IntProperty class
+---------------------------------------------------------------------*/
class OZN_IntProperty : public OZN_Property
{
public:
    OZN_IntProperty(OZN_PropertyId id, NPT_Int32 value);
    virtual ~OZN_IntProperty() {}

    // accessor methods
    OZN_PropertyType         GetType() { return OZN_PROPERTY_TYPE_INTEGER; }
    const OZN_PropertyValue& GetValue();
    NPT_Result               SetValue(const OZN_PropertyValue* value);

    // helper functions
    NPT_Result SetValue(NPT_Int32 value);
};

/*----------------------------------------------------------------------
|   OZN_StringProperty class
+---------------------------------------------------------------------*/
class OZN_StringProperty : public OZN_Property
{
public:
    OZN_StringProperty(OZN_PropertyId id, const char* value);
    virtual ~OZN_StringProperty() {}

    // accessor methods
    OZN_PropertyType         GetType() { return OZN_PROPERTY_TYPE_STRING; }
    const OZN_PropertyValue& GetValue();
    NPT_Result               SetValue(const OZN_PropertyValue* value);

    // helper functions
    NPT_Result SetValue(const char* value);

private:
    NPT_String m_Data;
};

/*----------------------------------------------------------------------
|   OZN_RawProperty class
+---------------------------------------------------------------------*/
class OZN_RawProperty : public OZN_Property
{
public:
    OZN_RawProperty(OZN_PropertyId id, NPT_AnyConst value, NPT_Size size);
    virtual ~OZN_RawProperty() {}

    // accessor methods
    OZN_PropertyType         GetType() { return OZN_PROPERTY_TYPE_RAW; }
    const OZN_PropertyValue& GetValue();
    NPT_Result               SetValue(const OZN_PropertyValue* value);

    // helper functions
    NPT_Result SetValue(NPT_AnyConst value, NPT_Size size);

private:
    NPT_DataBuffer m_Data;
};

/*----------------------------------------------------------------------
|   OZN_Properties class
+---------------------------------------------------------------------*/
class OZN_Properties
{
public:
    OZN_Properties();
    ~OZN_Properties();

    // methods
    NPT_Result CreateEmpty(const OZN_TableDescription& desc);
    NPT_Result CreateEmpty(const OZN_TableDescription& desc, OZN_PropertyId id);
    NPT_Result Create(const OZN_TableDescription* desc, ...);
    NPT_Result Create(const char* format, ...);
    NPT_Result Create(const char* format, va_list args);
    NPT_Result Clear();

    // accessor methods
    NPT_Result SetProperty(OZN_PropertyId id, NPT_Int32 value);
    NPT_Result SetProperty(OZN_PropertyId id, const char* value);
    NPT_Result SetProperty(OZN_PropertyId id, NPT_AnyConst value, NPT_Size size);
    
    const NPT_Array<OZN_Property*>& GetProperties() const { return m_Properties; }
    OZN_Property* GetProperty(OZN_PropertyId id) const;
    NPT_Result    FindProperty(OZN_PropertyId id, OZN_Property*& property) const;

private:
    NPT_Array<OZN_Property*> m_Properties;
};

#endif /* _OZN_PROPERTY_H_ */