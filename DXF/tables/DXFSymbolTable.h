#ifndef __DXF_SYMBOL_TABLE_H__
#define __DXF_SYMBOL_TABLE_H__
#pragma once

#ifndef __DXF_DLLAPI_H__
	#include "../DXFDLLAPI.h"
#endif

#ifndef __DXF_OBJECT_H__
	#include "base/DXFObject.h"
#endif

#ifndef __DXF_OBJECTID_H__
	#include "base/DXFObjectID.h"
#endif

#ifndef __DXF_TABLE_VISITOR_H__
    #include "DXFTableVisitor.h"
#endif

#ifndef __DXF_RECORD_VISITOR_H__
    #include "DXFRecordVisitor.h"
#endif

#include "vector"

class DXFDLLAPI CDXFSymbolTable : public CDXFObject
{
// Construction/Destruction
protected:
    CDXFSymbolTable(const char *sObjectName) : CDXFObject(sObjectName) { }

public:
    virtual ~CDXFSymbolTable() { }

// Overrides
public:
    virtual void Accept(CDXFRecordVisitor &visitor) { }
    virtual void AcceptTableVis(CDXFTableVisitor &vis) { vis.VisitSymbolTable(*this); }
    virtual void Accept(CDXFObjectVisitorAbs &vis) override { vis.VisitSymbolTable(*this); }

// Operations
public:
    CDXFSymbolTableRecord *GetTableRecordByName(const char *sName);
	void GetTableRecordId(const char *sName, CDXFObjectID &id);

protected:
	void AddTableRecord(CDXFSymbolTableRecord *pRecord, CDXFDatabase *pDB) {
	    CDXFObjectID objectID;
	    AddTableRecordId(pRecord, objectID, pDB);
    }
	void AddTableRecordId(CDXFSymbolTableRecord *pRecord, CDXFObjectID &objectID, CDXFDatabase *pDB);

public:
    template <class _Function>
    void ForEach(_Function __f) const {
        for (const CDXFObjectID &obj_id : m_listRecords) {
            CDXFSymbolTableRecord *pRecord = (CDXFSymbolTableRecord *)obj_id.GetObject();
            __f(pRecord);
        }
    }

// Attributes
public:
	std::vector<CDXFObjectID> m_listRecords; // maybe map has to be used
};

#endif