#ifndef __DXF_LINE_TYPE_TABLE_H__
#define __DXF_LINE_TYPE_TABLE_H__
#pragma once

#ifndef __DXF_SYMBOL_TABLE_H__
    #include "DXFSymbolTable.h"
#endif

#ifndef __DXF_TABLE_VISITOR_H__
    #include "DXFTableVisitor.h"
#endif

class CDXFLineTypeTableRecord;

class CDXFLineTypeTable : public CDXFSymbolTable
{
// Construction/Destruction
public:
    CDXFLineTypeTable() : CDXFSymbolTable("LTYPE") { }
    virtual ~CDXFLineTypeTable() { }

// Overrides
public:
    virtual void AcceptTableVis(CDXFTableVisitor &vis) override { vis.VisitLineTypeTable(*this); }

// Operations
public:    
    void AddTableRecord(CDXFSymbolTableRecord *pRecord, CDXFDatabase *pDB) {
        CDXFSymbolTable::AddTableRecord(pRecord, pDB);
    }

    void AddTableRecordId(CDXFSymbolTableRecord *pRecord, CDXFObjectID &objectID, CDXFDatabase *pDB) {
        CDXFSymbolTable::AddTableRecordId(pRecord, objectID, pDB);
    }
   
    CDXFLineTypeTableRecord *GetLineTypeTableRecord(const char *sName) {
        CDXFSymbolTableRecord *pRec = CDXFSymbolTable::GetTableRecordByName(sName);
        CDXFLineTypeTableRecord *pRecord = (CDXFLineTypeTableRecord *)pRec;
        return pRecord;
    }
};

#endif