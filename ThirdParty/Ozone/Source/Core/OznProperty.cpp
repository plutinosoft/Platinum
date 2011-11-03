/*****************************************************************
|
|   Ozone - Properties
|
|   Copyright (c) 2004-2008, Plutinosoft, LLC.
|   Author: Sylvain Rebaud (sylvain@plutinosoft.com)
|
****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "OznProperty.h"
#include "OznSchema.h"

/*----------------------------------------------------------------------
|   OZN_IntProperty::OZN_IntProperty
+---------------------------------------------------------------------*/
OZN_IntProperty::OZN_IntProperty(OZN_PropertyId id, NPT_Int32 value) :
    OZN_Property(id)
{
    m_Value.integer = value;
}

/*----------------------------------------------------------------------
|   OZN_IntProperty::GetValue
+---------------------------------------------------------------------*/
const OZN_PropertyValue&
OZN_IntProperty::GetValue()
{
    return m_Value;
}

/*----------------------------------------------------------------------
|   OZN_IntProperty::SetValue
+---------------------------------------------------------------------*/
NPT_Result
OZN_IntProperty::SetValue(const OZN_PropertyValue* value)
{
    if (!value) return NPT_ERROR_INVALID_PARAMETERS;

    m_Value.integer = value->integer;
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_IntProperty::SetValue
+---------------------------------------------------------------------*/
NPT_Result
OZN_IntProperty::SetValue(NPT_Int32 value)
{
    m_Value.integer = value;
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_StringProperty::OZN_StringProperty
+---------------------------------------------------------------------*/
OZN_StringProperty::OZN_StringProperty(OZN_PropertyId id, const char* value) :
    OZN_Property(id),
    m_Data(value)
{
}

/*----------------------------------------------------------------------
|   OZN_StringProperty::GetValue
+---------------------------------------------------------------------*/
const OZN_PropertyValue&
OZN_StringProperty::GetValue()
{
    m_Value.string = m_Data.GetChars();
    return m_Value;
}

/*----------------------------------------------------------------------
|   OZN_StringProperty::SetValue
+---------------------------------------------------------------------*/
NPT_Result
OZN_StringProperty::SetValue(const OZN_PropertyValue* value)
{
    if (!value) return NPT_ERROR_INVALID_PARAMETERS;

    m_Data = value->string;
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_StringProperty::SetValue
+---------------------------------------------------------------------*/
NPT_Result
OZN_StringProperty::SetValue(const char* value)
{
    m_Data = value;
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_RawProperty::OZN_RawProperty
+---------------------------------------------------------------------*/
OZN_RawProperty::OZN_RawProperty(OZN_PropertyId id, 
                                 NPT_AnyConst   value, 
                                 NPT_Size       size) :
    OZN_Property(id),
    m_Data(value, size)
{
}

/*----------------------------------------------------------------------
|   OZN_RawProperty::GetValue
+---------------------------------------------------------------------*/
const OZN_PropertyValue&
OZN_RawProperty::GetValue()
{
    m_Value.raw_data.size = m_Data.GetDataSize();
    m_Value.raw_data.data = m_Data.GetDataSize()?m_Data.GetData():NULL;
    return m_Value;
}

/*----------------------------------------------------------------------
|   OZN_RawProperty::SetValue
+---------------------------------------------------------------------*/
NPT_Result
OZN_RawProperty::SetValue(const OZN_PropertyValue* value)
{
    if (!value) return NPT_ERROR_INVALID_PARAMETERS;

    m_Data.SetData((const NPT_Byte*)value->raw_data.data, value->raw_data.size);
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_RawProperty::SetValue
+---------------------------------------------------------------------*/
NPT_Result
OZN_RawProperty::SetValue(NPT_AnyConst value, NPT_Size size)
{
    if (size == 0) {
        return m_Data.SetDataSize(0);
    }
        
    return m_Data.SetData((const NPT_Byte*)value, size);
}

/*----------------------------------------------------------------------
|   OZN_Properties::OZN_Properties
+---------------------------------------------------------------------*/
OZN_Properties::OZN_Properties()
{
}

/*----------------------------------------------------------------------
|   OZN_Properties::~OZN_Properties
+---------------------------------------------------------------------*/
OZN_Properties::~OZN_Properties()
{
    Clear();
}

/*----------------------------------------------------------------------
|   OZN_Properties::Clear
+---------------------------------------------------------------------*/
NPT_Result
OZN_Properties::Clear()
{
    m_Properties.Apply(NPT_ObjectDeleter<OZN_Property>());
    m_Properties.Clear();
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_Properties::CreateEmpty
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Properties::CreateEmpty(const OZN_TableDescription& desc)
{
    /* reset existing properties first */
    Clear();

    for (NPT_Cardinal i=0; i<desc.property_descs_count; i++) {
        NPT_CHECK_SEVERE(CreateEmpty(desc, i));
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_Properties::CreateEmpty
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Properties::CreateEmpty(const OZN_TableDescription& desc, OZN_PropertyId id)
{
    if (id>=desc.property_descs_count) return NPT_ERROR_INVALID_PARAMETERS;

    switch(desc.property_descs[id].type) {
        case OZN_PROPERTY_TYPE_INTEGER:
            NPT_CHECK_SEVERE(SetProperty(id, 0));
            break;
        case OZN_PROPERTY_TYPE_STRING:
            NPT_CHECK_SEVERE(SetProperty(id, ""));
            break;
        case OZN_PROPERTY_TYPE_RAW:
            NPT_CHECK_SEVERE(SetProperty(id, NULL, 0));
            break;
        default:
            return NPT_ERROR_INTERNAL;
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_Properties::Create
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Properties::Create(const OZN_TableDescription* desc, ...)
{
    NPT_String format;
    va_list    args;

    NPT_CHECK_POINTER_FATAL(desc);

    va_start(args, desc);
    for (NPT_Cardinal i=0; i<desc->property_descs_count; i++) {
        switch(desc->property_descs[i].type) {
            case OZN_PROPERTY_TYPE_INTEGER:
                format += "d";
                break;
            case OZN_PROPERTY_TYPE_STRING:
                format += "s";
                break;
            case OZN_PROPERTY_TYPE_RAW:
                format += "r";
                break;
            default:
                return NPT_ERROR_INTERNAL;
        }
    }

    NPT_Result res = Create((const char*)format, args);
    va_end(args);

    return res;
}

/*----------------------------------------------------------------------
|   OZN_Properties::Create
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Properties::Create(const char* format, ...)
{
    va_list args;

    NPT_CHECK_POINTER_FATAL(format);

    va_start(args, format);
    NPT_Result res = Create(format, args);
    va_end(args);
    
    return res;
}

/*----------------------------------------------------------------------
|   OZN_Properties::Create
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Properties::Create(const char* format, va_list args)
{
    /* reset output params first */
    Clear();

    OZN_PropertyId id = 0;
    while (*format != '\0') {
        // integer
        if (*format == 'd') {
            int d = va_arg(args, int);
            SetProperty(id++, d);
        }
        // string
        else if (*format == 's') {
            char* s = va_arg(args, char *);
            SetProperty(id++, s);
        }
        // raw
        else if (*format == 'r') {
            void* r = (void*) va_arg(args, void *);
            int   d = va_arg(args, int);
            SetProperty(id++, r, d);
        }
        // unknown
        else {
            return NPT_ERROR_INVALID_PARAMETERS;
        }

        *format++;
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   OZN_Properties::SetProperty
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Properties::SetProperty(OZN_PropertyId id, NPT_Int32 value) 
{
    OZN_Property* property = GetProperty(id);
    if (property) {
        if (property->GetType() != OZN_PROPERTY_TYPE_INTEGER) 
            return NPT_ERROR_INVALID_FORMAT;

        return static_cast<OZN_IntProperty*>(property)->SetValue(value);
    }
    return m_Properties.Add(new OZN_IntProperty(id, value));
}

/*----------------------------------------------------------------------
|   OZN_Properties::SetProperty
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Properties::SetProperty(OZN_PropertyId id, const char* value) 
{
    OZN_Property* property = GetProperty(id);
    if (property) {
        if (property->GetType() != OZN_PROPERTY_TYPE_STRING) 
            return NPT_ERROR_INVALID_FORMAT;

        return static_cast<OZN_StringProperty*>(property)->SetValue(value);
    }
    return m_Properties.Add(new OZN_StringProperty(id, value));
}

/*----------------------------------------------------------------------
|   OZN_Properties::SetProperty
+---------------------------------------------------------------------*/
NPT_Result 
OZN_Properties::SetProperty(OZN_PropertyId id, NPT_AnyConst value, NPT_Size size) 
{
    OZN_Property* property = GetProperty(id);
    if (property) {
        if (property->GetType() != OZN_PROPERTY_TYPE_RAW) 
            return NPT_ERROR_INVALID_FORMAT;

        return static_cast<OZN_RawProperty*>(property)->SetValue(value, size);
    }
    return m_Properties.Add((new OZN_RawProperty(id, value, size)));
}

/*----------------------------------------------------------------------
|   OZN_Properties::GetProperty
+---------------------------------------------------------------------*/
OZN_Property* 
OZN_Properties::GetProperty(OZN_PropertyId id) const 
{
    for (NPT_Cardinal i=0; i<m_Properties.GetItemCount(); i++) {
        if (id == m_Properties[i]->GetId()) {
            return m_Properties[i];
        }
    }

    return NULL;
}
