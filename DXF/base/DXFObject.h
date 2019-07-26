#ifndef __DXF_OBJECT_H__
#define __DXF_OBJECT_H__
#pragma once

#ifndef __DXF_OBJECTID_H__
	#include "DXFObjectID.h"
#endif

#include "string"

class CDXFObjectVisitorAbs
{
    friend class CDXFDictionary;
    friend class CDXFMlineStyle;
    friend class CDXFPlaceHolder;
    friend class CDXFEntity;
    friend class CDXFSymbolTableRecord;
    friend class CDXFSymbolTable;

protected:
    CDXFObjectVisitorAbs() { }
    virtual ~CDXFObjectVisitorAbs() { }

protected:
    virtual void VisitDictionary(CDXFDictionary &dict) = 0;
    virtual void VisitDXFLineStyle(CDXFMlineStyle &line) = 0;
    virtual void VisitDXFPlaceHolder(CDXFPlaceHolder &placeholder) = 0;
    virtual void VisitDXFEntity(CDXFEntity &entity) = 0;
    virtual void VisitDXFSymbolTableRecord(CDXFSymbolTableRecord &record) = 0;
    virtual void VisitSymbolTable(CDXFSymbolTable &table) = 0;
};

class CDXFObject
{
	friend class CDXFDatabase;

// Construction/Destruction
protected:
    CDXFObject() = delete;
    CDXFObject(const char *sObjectName) : m_sObjectName(sObjectName) { }
    virtual ~CDXFObject() { }

// Operations
public:
    void SetOwnerID(const CDXFObjectID &ownerID) { m_ownerID = ownerID; }
    const CDXFObjectID &GetObjectID() const { return m_objectID; }
    const CDXFObjectID &GetOwnerID() const { return m_ownerID; }

    unsigned long GetHandle() const { return m_objectID.GetHandle(); }
    const char *GetObjectName() const { return m_sObjectName.c_str(); }

// Overrides
public:
    virtual void Accept(CDXFObjectVisitorAbs &vis) = 0;

// Attributes
public:
	CDXFObjectID m_objectID;
	CDXFObjectID m_ownerID;

private:
    std::string m_sObjectName;
};

#endif
