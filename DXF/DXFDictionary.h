#ifndef __DXF_DICTIONARY_H__
#define __DXF_DICTIONARY_H__
#pragma once

#ifndef __DXF_OBJECT_H__
	#include "base/DXFObject.h"
#endif

#include "unordered_map"

class CDXFDictionaryVisitorAbs
{
    friend class CDXFDictionaryWithDefault;
    friend class CDXFDictionary;

protected:
    CDXFDictionaryVisitorAbs() { }
    virtual ~CDXFDictionaryVisitorAbs() { }

protected:
    virtual void VisitDictionary(CDXFDictionary &dict)                       = 0;
    virtual void VisitDictionaryWithDefault(CDXFDictionaryWithDefault &dict) = 0;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CDXFDictionary
/////////////////////////////////////////////////////////////////////////////////////////////////////////
class CDXFDictionary : public CDXFObject  
{
// Construction/Destruction
public:
    CDXFDictionary(const char *sObjectName) : CDXFObject(sObjectName) { }
    CDXFDictionary() : CDXFObject("DICTIONARY") { }
    virtual ~CDXFDictionary() { }

// Operations
public:
	bool SetAt(const char *sName, CDXFObject *pObject, CDXFDatabase *pDatabase);
	bool SetAt(const char *sName, CDXFObject *pObject, CDXFObjectID &objectID, CDXFDatabase *pDatabase);

    CDXFObject *GetObjectByName(const char *sName) {
	    auto found = m_map.find(sName);
	    if ( found != m_map.end() ) {
		    return found->second;
	    }
        return nullptr;
    }

// Overrides
public:
    virtual void Accept(CDXFDictionaryVisitorAbs &vis) { vis.VisitDictionary(*this); }
    virtual void Accept(CDXFObjectVisitorAbs &vis) override { vis.VisitDictionary(*this); }

// Attributes
public:
	std::unordered_map<std::string, CDXFObject *> m_map;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CDXFDictionaryWithDefault
/////////////////////////////////////////////////////////////////////////////////////////////////////////
class CDXFDictionaryWithDefault : public CDXFDictionary
{
// Construction/Destruction
public:
    CDXFDictionaryWithDefault() : CDXFDictionary("ACDBDICTIONARYWDFLT") { }
    virtual ~CDXFDictionaryWithDefault() { }
 
// Operations
public:
    void SetDefaultID(const CDXFObjectID &defaultID) { m_defaultID = defaultID; }
    const CDXFObjectID &GetDefaultID() const { return m_defaultID; }

// Overrides  
public:
    virtual void Accept(CDXFDictionaryVisitorAbs &vis) override { vis.VisitDictionaryWithDefault(*this); }

// Attributes 
private:
	CDXFObjectID m_defaultID;
};

#endif